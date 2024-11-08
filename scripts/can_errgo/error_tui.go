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

	component "main/components"

	"github.com/charmbracelet/bubbles/key"
	"github.com/charmbracelet/bubbles/table"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
	"golang.org/x/sys/unix"
)


const (
	canInterface = "vcan0"  // Use the virtual CAN interface
	canFrameSize = 16       // CAN frame size (for a standard CAN frame)
)

func deepCopy(rows []table.Row) []table.Row {
    newRows := make([]table.Row, len(rows))
    for i := range rows {
        newRows[i] = make(table.Row, len(rows[i]))
        copy(newRows[i], rows[i])
    }
    return newRows
}

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

type TickMsg struct{}

type model struct {
    table        table.Model
	tableKeys	 KeyMap	

	submenuTable  table.Model
	submenuActive bool
	submenuKeys   KeyMap

    hiddenCounts map[string]int
    errorToBit   map[string]int
    ignoreMask   uint64

	isTimeout   bool
	lastCANTime time.Time
	t           time.Duration

	box component.Box
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

func findRowString(s []table.Row, e string) int {
    for i, a := range s {
        if a[0] == e {
            return i
        }
    }
    return -1
}

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

func (m *model) SortByColumn(columnIndex int) error {
    rows := m.table.Rows()

    // Check if sorting should adjust cursor position
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
                m.table.SetCursor(i)
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

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd
	switch msg := msg.(type) {
	case tea.KeyMsg:
		if m.submenuActive{
			// If submenu is active, only let the submenu handle KeyMsg
			switch msg.String() {
			case "up":
				if overBounds(m.submenuTable){
					m.table.SetCursor(0)
					return m, nil
				}
				m.submenuTable.MoveUp(1)			
				// m.box.SetText(m.submenuTable.SelectedRow()[0], "temp description for subtable")
				
				return m,nil	
			case "down":
				if overBounds(m.submenuTable){
					m.table.SetCursor(0)
					return m, nil
				}	
				m.submenuTable.MoveDown(1)		
				// m.box.SetText(m.submenuTable.SelectedRow()[0], "temp description for subtable")
				return m,nil			
			case "i":
				if overBounds(m.submenuTable){
					m.table.SetCursor(0)
					return m, nil
				}
				
				if m.submenuTable.SelectedRow() == nil{
					return m, nil
				}
				
				m.ignoreMask = toggleBit(m.ignoreMask, m.errorToBit[m.submenuTable.SelectedRow()[0]])
				newRows := deleteElementRow(m.submenuTable.Rows(), m.submenuTable.Cursor())
				m.submenuTable.SetRows(newRows)
				if m.submenuTable.Cursor() == 0{
					m.submenuTable.MoveDown(1)
				}else{
					m.submenuTable.MoveUp(1)
				}

			case "s":
				m.submenuActive = false  // Hide submenu
				m.table.Focus()  
				m.submenuTable.Blur() 

				if m.table.SelectedRow() == nil{
					m.table.SetCursor(0)
					return m, nil
				}
				// m.box.SetText(m.table.SelectedRow()[0], "temp description for main table")     
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

			if overBounds(m.table){
				m.table.SetCursor(0)
				return m, nil
			}
			
			if m.table.SelectedRow() == nil{
				m.table.SetCursor(0)
				return m, nil
			}

			m.hiddenCounts[m.table.SelectedRow()[0]] = 0
			newRows := deleteElementRow(m.table.Rows(), m.table.Cursor())
			m.table.SetRows(newRows)
			if m.table.Cursor() == 0{
				m.table.MoveDown(1)
			}else{
				m.table.MoveUp(1)
			}
			return m, nil
		case "i":
			
			if overBounds(m.table){
				m.table.SetCursor(0)
				return m, nil
			}
            
			if m.table.SelectedRow() == nil{
				m.table.SetCursor(0)
				return m, nil
			}
			
			m.ignoreMask = toggleBit(m.ignoreMask, m.errorToBit[m.table.SelectedRow()[0]] )
			newSubRow := append(m.submenuTable.Rows(), table.Row{m.table.SelectedRow()[0]})
			m.submenuTable.SetRows(newSubRow)
			m.submenuTable.UpdateViewport()

			m.hiddenCounts[m.table.SelectedRow()[0]] = 0
			newRows := deleteElementRow(m.table.Rows(), m.table.Cursor())
			m.table.SetRows(newRows)
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
            }
			if len(m.submenuTable.Rows()) > 0{
				m.submenuTable.SetCursor(0)
				// m.box.SetText(m.submenuTable.SelectedRow()[0], "temp description for subtable")     
			}
		case "up":
			if overBounds(m.table){
				m.table.SetCursor(0)
				return m, nil
			}		

			if m.table.SelectedRow() == nil{
				m.table.SetCursor(0)
				return m, nil
			}

			m.table.MoveUp(1)
			// m.box.SetText(m.table.SelectedRow()[0], "temp description for main table")
			return m,nil	
		case "down":
			if overBounds(m.table){
				m.table.SetCursor(0)
				return m, nil
			}			

			if m.table.SelectedRow() == nil{
				m.table.SetCursor(0)
				return m, nil
			}

			m.table.MoveDown(1)
			// m.box.SetText(m.table.SelectedRow()[0], "temp description for main table")
			return m,nil			
	}
	case CANMsg:
		m.lastCANTime = time.Now()
		m.isTimeout = false
		msg.Value = msg.Value & m.ignoreMask
		m.re_fresh()
		for k := 0; k < 64; k++ { // Iterate through all 64 bits
			if msg.Value&(1<<k) != 0 { // Check if the k-th bit is set

				var errorNumberStr string = strconv.Itoa(k)

				if val, ok := m.hiddenCounts["error" + errorNumberStr]; ok{
					m.hiddenCounts["error" + errorNumberStr ] = val + 1
					
				} else{
					m.hiddenCounts["error" + errorNumberStr] = 1
					m.errorToBit["error" + errorNumberStr] = k
				}

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
			m.SortByColumn(2)
			m.table.UpdateViewport()
		}
		return m, nil
	case TickMsg:
		if time.Since(m.lastCANTime) > m.t {
			m.isTimeout = true
		}
		return m, tickEvery(m.t)
	}

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

func (m model) View() string {
	out := baseStyle.Render(m.table.View()) + "\n"
	if m.isTimeout{
		out = fmt.Sprint("\n\x1b[41m\x1b[37mWarning! Last recorded message was over ",math.Round(time.Since(m.lastCANTime).Seconds()), " seconds ago!\x1b[0m") + "\n" +  out
	}
	if m.submenuActive{
		if m.submenuTable.SelectedRow() != nil{
			m.box.SetText(m.submenuTable.SelectedRow()[0], "temp description for sub table")
		}
		out = out + m.box.View();
        out =  out + "\n" +
               baseStyle.Render(m.submenuTable.View()) + "\n" + 
			   m.table.HelpView() +  "\n" +
			   m.submenuTable.Help.ShortHelpView(m.submenuKeys.ShortHelp()) +  "\n"
	}else{
		if m.table.SelectedRow() != nil {
			m.box.SetText(m.table.SelectedRow()[0], "temp description for main table")
		}
		out = out + m.box.View()
		out = out + "\n" + 
			m.table.HelpView() + "\n" + 
			m.table.Help.ShortHelpView(m.tableKeys.ShortHelp()) + "\n"
		}
	// if m.showBox{
	// 	out = out + m.box.View();
	// }
    // if m.submenuActive {
    //     // Display both the main menu and submenu
    //     out =  out + "\n" +
    //            baseStyle.Render(m.submenuTable.View()) + "\n" + 
	// 		   m.table.HelpView() +  "\n" +
	// 		   m.submenuTable.Help.ShortHelpView(m.submenuKeys.ShortHelp()) +  "\n"
    // }else{
    // out = out + "\n" + 
	// 	m.table.HelpView() + "\n" + 
	// 	m.table.Help.ShortHelpView(m.tableKeys.ShortHelp()) + "\n"
	// }
	return out;
}

func  createTable(warning int) model{

	columns := []table.Column{
		{Title: "Error:", Width: 40},
		{Title: "Count:", Width: 10},
		{Title: "Freshness", Width: 10},

	}
	rows := []table.Row {}
	

	t := table.New(
		table.WithColumns(columns),
		table.WithRows(rows),
		table.WithFocused(true),
		table.WithHeight(7),
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

	table2 := createSubMenu()

	box := component.Box{};
	box.SetWidth(60)

	m := model{
		t,
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

// Initialization of the submenu
func createSubMenu() table.Model {
    columns := []table.Column{
        {Title: "Ignored Errors:", Width: 30},
    }

    rows := []table.Row{}
	t := table.New(
		table.WithColumns(columns),
		table.WithRows(rows),
		table.WithFocused(true),
		table.WithHeight(5),
	)

	s := table.DefaultStyles()
	s.Header = s.Header.	
		BorderStyle(lipgloss.NormalBorder()).
		BorderForeground(lipgloss.Color("240")).
		BorderBottom(true).
		Bold(false)
	s.Selected = s.Selected.
		Foreground(lipgloss.Color("112")).
		Background(lipgloss.Color("240")).
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
		warning = 999 // Default value
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

	var m model = createTable(warning)
	quit := make(chan struct{})

	p := tea.NewProgram(m)
	go func() {
		if _, err := p.Run(); err != nil {
			fmt.Println("Error running TUI:", err)
		}
		close(quit)
	}()
	go can_listener(p)


	<-quit
}
