package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"math"
	"net"
	"os"
	"sort"
	"strconv"
	"time"

	box "main/components/box"
	table "main/components/table"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
	"golang.org/x/sys/unix"
)


const (
	canInterface = "vcan0"  // Use the virtual CAN interface
	canFrameSize = 16       // CAN frame size (for a standard CAN frame)
)

// Might have future use
func deepCopy(rows []table.Row) []table.Row {
    newRows := make([]table.Row, len(rows))
    for i := range rows {
        newRows[i] = make(table.Row, len(rows[i]))
        copy(newRows[i], rows[i])
    }
    return newRows
}

// Converts a 8-byte slice to a uint64, this is for masking purposes.
func bytesToUint64(b []byte) uint64 {
    var result uint64
    for i := 0; i < len(b) && i < 8; i++ {
        // Reverse the order: least significant byte (rightmost) gets the lowest bits
        result |= uint64(b[len(b)-1-i]) << (8 * i)
    }
    return result
}

var baseStyle = lipgloss.NewStyle().
	BorderStyle(lipgloss.NormalBorder()).
	BorderForeground(lipgloss.Color("240"))


type CANMsg struct {
	ID    uint32
	Value uint64
}

// Empty type for the tick message
type TickMsg struct{}

type model struct {
	// Main table
    table        table.Model
	tableKeys	 KeyMap	

	// Submenu table
	submenuTable  table.Model
	submenuActive bool
	submenuKeys   KeyMap

	// Hidden error counts and error to bit mapping
    hiddenCounts map[string]int
    errorToBit   map[string]int
    ignoreMask   uint64

	// Timeout flag and last CAN message time
	isTimeout   bool
	lastCANTime time.Time
	t           time.Duration

	// Box for displaying error descriptions
	box box.Box
	showBox bool
}


// toggleBit toggles the specified bit in the number
//If the bit is 0, it sets it to 1; if it is 1, it sets it to 0.
func toggleBit(n uint64, bitPosition int) uint64 {
    mask := uint64(1) << bitPosition
    return n ^ mask
}


func deleteElementRow(slice []table.Row, index int) []table.Row {
    // Create a new slice with the same length minus 1
    newSlice := make([]table.Row, 0, len(slice)-1)
                      
    // Append the elements before the index
    newSlice = append(newSlice, slice[:index]...)

    // Append the elements after the index
    newSlice = append(newSlice, slice[index+1:]...)

    return newSlice
}

func tickEvery(t time.Duration) tea.Cmd {
	return func() tea.Msg {
		time.Sleep(t)
		return TickMsg{}
	}
}

// Might have future use
func findRow(s []table.Row, e table.Row) int {
    for i, a := range s {
        if a[0] == e[0] {
            return i
        }
    }
    return -1
}

// Find the index of a row with a specific error name.
func findRowString(s []table.Row, e string) int {
    for i, a := range s {
        if a[0] == e {
            return i
        }
    }
    return -1
}

// Check if the cursor is over bounds
func overBounds(table table.Model) bool{
	return len(table.Rows())==0 || table.Cursor() < 0 || table.Cursor() >= len(table.Rows()) 
}


func (m *model) re_fresh(){
	rows := m.table.Rows()
	for i:=0; i < len(rows); i++{
		if val,err := strconv.Atoi(rows[i][2]); err != nil{
			fmt.Printf("Something went wrong during refresh!")
		} else{
			rows[i][2] = strconv.Itoa(val + 1)
		}
	}
}

// Sort the table by the specified column index.
func (m *model) SortByColumn(columnIndex int) error {
    rows := m.table.Rows()
    // Check if sorting should adjust cursor position (In the odd case that no cursor is set, we don't want to run into a panic)
    if !overBounds(m.table) {
        // Capture the current row data before sorting
        currentIndex := m.table.Cursor()
        selectedRow := rows[currentIndex]
        
        // Perform sorting with a custom less function
        sort.SliceStable(rows, func(i, j int) bool {
            if columnIndex >= len(rows[i]) || columnIndex >= len(rows[j]) {
                fmt.Println("Error: columnIndex out of bounds")
                return false
            }

            val1, err1 := strconv.Atoi(rows[i][columnIndex])
            val2, err2 := strconv.Atoi(rows[j][columnIndex])

            if err1 == nil && err2 == nil {
                return val1 < val2
            }
            return rows[i][columnIndex] < rows[j][columnIndex]
        })

        // Find new index of the originally selected row
        for i, row := range rows {
            if row[0] == selectedRow[0] {
                m.table.SetCursorAndViewport(i)
                break
            }
        }
		
    } else {
        // Just sort if out of bounds
        sort.SliceStable(rows, func(i, j int) bool {
            if columnIndex >= len(rows[i]) || columnIndex >= len(rows[j]) {
                fmt.Println("Error: columnIndex out of bounds")
                return false
            }

            val1, err1 := strconv.Atoi(rows[i][columnIndex])
            val2, err2 := strconv.Atoi(rows[j][columnIndex])

            if err1 == nil && err2 == nil {
                return val1 < val2
            }
            return rows[i][columnIndex] < rows[j][columnIndex]
        })
    }
    return nil
}


func (m model) Init() tea.Cmd { return tickEvery(m.t) }


// Table and ignore subtable update loop.
func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd
	switch msg := msg.(type) {
	case tea.KeyMsg:
		if m.submenuActive{
			// If submenu is active, only let the submenu handle KeyMsg
			switch msg.String() {
			case "up":
				m.submenuTable.MoveUp(1)			
				return m,nil	
			case "down":
				m.submenuTable.MoveDown(1)		
				return m,nil			
			case "i":
				if m.submenuTable.SelectedRow() == nil{
					return m, nil
				}
				
				// Toggle the mask for the selected error and delete the row. 
				m.ignoreMask = toggleBit(m.ignoreMask, m.errorToBit[m.submenuTable.SelectedRow()[0]])
				newRows := deleteElementRow(m.submenuTable.Rows(), m.submenuTable.Cursor())
				m.submenuTable.SetRows(newRows)

				// May be unnecessary, but just in case we update the cursor.
				if m.submenuTable.Cursor() == 0{
					m.submenuTable.MoveDown(1)
				}else{
					m.submenuTable.MoveUp(1)
				}

			case "s":
				m.submenuActive = false  // Hide submenu
				m.table.Focus()  // Refocus the main table
				m.submenuTable.Blur() // Unfocus the submenu
				m.table.SetShowCursor(true) // Reshow the cursor on the main table

				return m, nil
	
			case "q", "ctrl+c":
				return m, tea.Quit
			}

			m.submenuTable, cmd = m.submenuTable.Update(msg)
			return m, cmd
		}


		switch msg.String() {
		case "esc":
			if m.table.Focused() {
				m.table.Blur()
			} else {
				m.table.Focus()
			}
		case "q", "ctrl+c":
			return m, tea.Quit
		case "a":	
			
			// Prevent panic if no row is selected
			if m.table.SelectedRow() == nil{
				m.table.SetCursorAndViewport(0)
				return m, nil
			}

			// Remove the row and reset its count.
			m.hiddenCounts[m.table.SelectedRow()[0]] = 0
			newRows := deleteElementRow(m.table.Rows(), m.table.Cursor())
			m.table.SetRows(newRows)

			// May be unnecessary, but just in case we update the cursor.
			if m.table.Cursor() == 0{
				m.table.MoveDown(1)
			}else{
				m.table.MoveUp(1)
			}
			return m, nil
		case "i":

			// Prevent panic if no row is selected
			if m.table.SelectedRow() == nil{
				m.table.SetCursorAndViewport(0)
				return m, nil
			}

			// Toggle the mask for the selected error and move it to the ignore menu.
			m.ignoreMask = toggleBit(m.ignoreMask, m.errorToBit[m.table.SelectedRow()[0]] )
			newSubRow := append(m.submenuTable.Rows(), table.Row{m.table.SelectedRow()[0]})
			m.submenuTable.SetRows(newSubRow)
			m.submenuTable.UpdateViewport()

			// Remove the row from the main table and reset its count.
			m.hiddenCounts[m.table.SelectedRow()[0]] = 0
			newRows := deleteElementRow(m.table.Rows(), m.table.Cursor())
			m.table.SetRows(newRows)

			// May be unnecessary, but just in case we update the cursor.
			m.table.MoveDown(1)

			return m, nil
        case "s":
            if m.submenuActive {
                // Hide submenu and return to main menu
                m.submenuActive = false
                m.table.Focus()
            } else {
                // Show submenu and focus it
                m.submenuActive = true
                m.table.Blur()
                m.submenuTable.Focus()
				m.table.SetShowCursor(false)
            }
		case "up":
			m.table.MoveUp(1)
			return m,nil	
		case "down":
			m.table.MoveDown(1)
			return m,nil			
	}
	case CANMsg:
		// Reset the timeout flag and update the last CAN message time
		m.lastCANTime = time.Now()
		m.isTimeout = false
		// Perform bitwise AND to filter out the bits that are not of interest
		msg.Value = msg.Value & m.ignoreMask
		// Update the freshness of the errors
		m.re_fresh()


		for k := 0; k < 64; k++ { // Iterate through all 64 bits
			if msg.Value&(1<<k) != 0 { // Check if the k-th bit is set

				var errorNumberStr string = strconv.Itoa(k)

				// Instead of "error" + #, this would be what the DBC file should map to, for now keep this for testing
				if val, ok := m.hiddenCounts["error" + errorNumberStr]; ok{
					m.hiddenCounts["error" + errorNumberStr ] = val + 1
					
				} else{
					m.hiddenCounts["error" + errorNumberStr] = 1
					m.errorToBit["error" + errorNumberStr] = k
				}

				// If the error is not in the table, add it, otherwise update the count.
				index := findRowString(m.table.Rows(),  "error" + errorNumberStr)
				if index == -1{
					newRow := append(m.table.Rows(), table.Row{ "error" + errorNumberStr, strconv.Itoa(m.hiddenCounts["error" + errorNumberStr]),"0"})
					m.table.SetRows(newRow)
				} else{
					// Potentially need deep copy. If there is new error come back to this.
					newRows := m.table.Rows()
					newRows[index][1] = strconv.Itoa(m.hiddenCounts[newRows[index][0]])
					newRows[index][2] = "0"
					m.table.SetRows(newRows)
				}
			}
			
			// Sort the table by the freshness.
			m.SortByColumn(2)
			m.table.UpdateViewport()
		}
		return m, nil
	// If the message is a tick, check if the last CAN message was received more than the timeout duration ago
	case TickMsg:
		if time.Since(m.lastCANTime) > m.t {
			m.isTimeout = true
		}
		return m, tickEvery(m.t)
	}

	// Update the table with the new message
	if _, ok := msg.(CANMsg); ok {
        // Update the mainTable regardless of submenu state
        m.table, cmd = m.table.Update(msg)
    }else if m.submenuActive {
        m.submenuTable, cmd = m.submenuTable.Update(msg)
    } else {
        m.table, cmd = m.table.Update(msg)
    }

	return m, cmd
}

// KeyMap is a set of keybindings for the application.
type KeyMap struct {
	a       key.Binding
	i       key.Binding
	s	    key.Binding
	q       key.Binding

}

// ShortHelp implements the KeyMap interface.
func (km KeyMap) ShortHelp() []key.Binding {
	return []key.Binding{km.a, km.i, km.s, km.q}
}


// DefaultKeyMap returns a default set of keybindings.
func additionalKeyMap() KeyMap {
	return KeyMap{
		a: key.NewBinding(
			key.WithKeys("a"),
			key.WithHelp("a", "Acknowledge"),
		),
		i: key.NewBinding(
			key.WithKeys("i"),
			key.WithHelp("i", "Ignore"),
		),
		s: key.NewBinding(
			key.WithKeys("s"),
			key.WithHelp("s", "Ignored Menu"),
		),
		q: key.NewBinding(
			key.WithKeys("q"),
			key.WithHelp("q/ctrl+c", "quit"),
		),
	}
}

func subMenuKeyMap() KeyMap {
	return KeyMap{
		i: key.NewBinding(
			key.WithKeys("i"),
			key.WithHelp("i", "Unignore"),
		),
		s: key.NewBinding(
			key.WithKeys("s"),
			key.WithHelp("s", "Close Ignored Menu"),
		),
		q: key.NewBinding(
			key.WithKeys("q"),
			key.WithHelp("q/ctrl+c", "quit"),
		),
	}
}

// View returns the string representation of the model. We could combine and condense, but its clearer to keep it separate.
func (m model) View() string {

	out := "\n \n \n"
	// Help menu changes based on the state of the submenu
	if m.submenuActive{
		out = 	m.table.HelpView() + "\n" + 
				m.submenuTable.Help.ShortHelpView(m.submenuKeys.ShortHelp()) + "\n"
	}else{
		out = 	m.table.HelpView() + "\n" +
				m.table.Help.ShortHelpView(m.tableKeys.ShortHelp()) + "\n"	
	}

	// Timeout warning
	if m.isTimeout{
		out = fmt.Sprint("\n\x1b[41m\x1b[37mWarning! Last recorded message was over ",math.Round(time.Since(m.lastCANTime).Seconds()), " seconds ago!\x1b[0m") + "\n" +  out
	}

	// Render the table 
	out = out + baseStyle.Render(m.table.View()) + "\n"

	// Render the submenu if it is active
	if m.submenuActive{
		out = out + baseStyle.Render(m.submenuTable.View()) + "\n"
	}

	// Render the box 
	if m.submenuActive && m.submenuTable.SelectedRow() != nil{
		m.box.SetText(m.submenuTable.SelectedRow()[0], "temp description for " + m.submenuTable.SelectedRow()[0])
		out = out + m.box.View()
	} else if m.table.SelectedRow() != nil && !m.submenuActive{
		m.box.SetText(m.table.SelectedRow()[0], "temp description for " + m.table.SelectedRow()[0])
		out = out + m.box.View()
	}

	return out
}

func initViewer(warning int) model{

	mainTable := createTable()
	table2 := createSubMenu()

	box := box.Box{};
	// Each column has padding of 2, so the width is 60 + 2*3(error columns) = 66
	box.SetWidth(66)

	m := model{
		mainTable,
		additionalKeyMap(),
		
		table2,
		false,
		subMenuKeyMap(),
		
		map[string]int{},
		map[string]int{},
		^uint64(0),
		
		false,
		time.Now(),
		time.Duration(warning) * time.Second,
		
		box,
		false,
	}
	return m
}

// Default setup for the main table
func createTable() table.Model{

	// True Width = 60 + 2(padding) * (# cols) = 66
	columns := []table.Column{
		{Title: "Error:", Width: 40},
		{Title: "Count:", Width: 10},
		{Title: "Recency", Width: 10},

	}
	rows := []table.Row {}
	

	t := table.New(
		table.WithColumns(columns),
		table.WithRows(rows),
		table.WithFocused(true),
		// The header will take up 1 row. If we want N rows, we need to set the height to N+1
		table.WithHeight(11),
	)

	s := table.DefaultStyles()
	s.Header = s.Header.	
		BorderStyle(lipgloss.NormalBorder()).
		BorderForeground(lipgloss.Color("240")).
		BorderBottom(true).
		Bold(false)
	s.Selected = s.Selected.
		Foreground(lipgloss.Color("229")).
		Background(lipgloss.Color("57")).
		Bold(false)
	t.SetStyles(s)
	return t
}

// Initialization of the submenu
func createSubMenu() table.Model {
	// Submenu has 1 column meaning it's true width is 60 + 2(padding) = 62, we need to account for the padding of the main table (66), so we add 4.
    columns := []table.Column{
        {Title: "Ignored Errors:", Width: 60 + 4} ,
    }

    rows := []table.Row{}
	t := table.New(
		table.WithColumns(columns),
		table.WithRows(rows),
		table.WithFocused(true),
		// The header will take up 1 row. If we want N rows, we need to set the height to N+1
		table.WithHeight(5),
	)

	s := table.DefaultStyles()
	s.Header = s.Header.	
		BorderStyle(lipgloss.NormalBorder()).
		BorderForeground(lipgloss.Color("240")).
		BorderBottom(true).
		Bold(false)
	s.Selected = s.Selected.
		Foreground(lipgloss.Color("229")).
		Background(lipgloss.Color("57")).
		Bold(false)
	t.SetStyles(s)

    return t
}


func can_listener(p *tea.Program) {
	// Create a raw CAN socket using the unix package
	var sock int;
	var err error;
	
	sock, err = unix.Socket(unix.AF_CAN, unix.SOCK_RAW, unix.CAN_RAW)
	if err != nil {
		log.Fatalf("Error creating CAN socket: %v", err)
	}
	defer unix.Close(sock)

	// Get the index of the interface (vcan0)
	ifi, err := net.InterfaceByName(canInterface)
	if err != nil {
		log.Fatalf("Error getting interface index: %v", err)
	}

	// Bind the socket to the CAN interface
	addr := &unix.SockaddrCAN{Ifindex: ifi.Index}
	if err := unix.Bind(sock, addr); err != nil {
		log.Fatalf("Error binding CAN socket: %v", err)
	}

	// Create a buffer to hold incoming CAN frames
	buf := make([]byte, canFrameSize)

	fmt.Println("Listening for CAN messages on", canInterface)

	for {
		// Receive CAN message
		_, err := unix.Read(sock, buf)
		if err != nil {
			log.Printf("Error receiving CAN message: %v", err)
			continue
		}

		// Extract CAN ID and data
		canID := binary.LittleEndian.Uint32(buf[0:4]) & 0x1FFFFFFF // 29-bit CAN ID (masked)
		data := bytesToUint64(buf[8:]) 
		
		msg := CANMsg{ ID : canID, Value: data}
		p.Send(msg)
	}
}


func main() {
	// Define the command line flag
	warnFlag := flag.String("w", "", "warning time for the table (integer value)")

	flag.Parse()

	var warning int
	if *warnFlag == "" {
		warning = 5 // Default value
	} else {
		// Parse the value for the -w flag
		var err error
		warning, err = strconv.Atoi(*warnFlag)
		if err != nil {
			// If it's not a valid integer, show the usage message and exit
			fmt.Println("Usage: [program name] -w [int]")
			os.Exit(1)
		}
	}

	// Initialize the model and run the program
	var m model = initViewer(warning)
	// channel to listen for the quit signal
	quit := make(chan struct{})

	// Start the TUI
	p := tea.NewProgram(m)
	go func() {
		if _, err := p.Run(); err != nil {
			fmt.Println("Error running TUI:", err)
		}
		close(quit)
	}()
	// Start the CAN listener
	go can_listener(p)


	<-quit
}
