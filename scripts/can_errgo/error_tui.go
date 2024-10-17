package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"net"

	"strconv"

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
		result |= uint64(b[i]) << (8 * i)
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

type model struct {
    table        table.Model
	tableKeys	 KeyMap	

	submenuTable  table.Model
	submenuActive bool
	submenuKeys   KeyMap

    hiddenCounts map[string]int
    errorToBit   map[string]int
    ignoreMask   uint64
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

func  overBounds(table table.Model) bool{
	return len(table.Rows())==0 || table.Cursor() < 0 || table.Cursor() >= len(table.Rows()) 
}


func (m model) Init() tea.Cmd { return nil }

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd
	switch msg := msg.(type) {
	case tea.KeyMsg:
		if m.submenuActive{
			// If submenu is active, only let the submenu handle KeyMsg
			switch msg.String() {
			case "i":
				if overBounds(m.submenuTable){
					return m, nil
				}
	
				m.ignoreMask = toggleBit(m.ignoreMask, m.errorToBit[m.submenuTable.SelectedRow()[0]])
				newRows := deleteElementRow(m.submenuTable.Rows(), m.submenuTable.Cursor())
				m.submenuTable.SetRows(newRows)

			case "s":
				m.submenuActive = false  // Hide submenu
				m.table.Focus()  
				m.submenuTable.Blur()        
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
				return m, nil
			}


			m.hiddenCounts[m.table.SelectedRow()[0]] = 0
			newRows := deleteElementRow(m.table.Rows(), m.table.Cursor())
			m.table.SetRows(newRows)
			m.table.UpdateViewport()
			return m, nil
		case "i":
			
			if overBounds(m.table){
				return m, nil
			}
                        
			
			m.ignoreMask = toggleBit(m.ignoreMask, m.errorToBit[m.table.SelectedRow()[0]] )
			newSubRow := append(m.submenuTable.Rows(), table.Row{m.table.SelectedRow()[0]})
			m.submenuTable.SetRows(newSubRow)
			m.submenuTable.UpdateViewport()

			m.hiddenCounts[m.table.SelectedRow()[0]] = 0
			newRows := deleteElementRow(m.table.Rows(), m.table.Cursor())
			m.table.SetRows(newRows)
			m.table.UpdateViewport()

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
	}
	case CANMsg:
		msg.Value = msg.Value & m.ignoreMask
		for k := 0; k < 64; k++ { // Iterate through all 32 bits
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
					newRow := append(m.table.Rows(), table.Row{ "error" + errorNumberStr, strconv.Itoa(m.hiddenCounts["error" + errorNumberStr]) })
					m.table.SetRows(newRow)
				} else{
					newRows := deepCopy(m.table.Rows())
					newRows[index][1] = strconv.Itoa(m.hiddenCounts[newRows[index][0]])
					m.table.SetRows(newRows)
				}

				m.table.UpdateViewport()
			}
		}
		return m, nil
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




func (m model) View() string {
    if m.submenuActive {
        // Display both the main menu and submenu
        return baseStyle.Render(m.table.View()) + "\n\n" +
               baseStyle.Render(m.submenuTable.View()) + "\n" + 
			   m.table.HelpView() +  "\n" +
			   m.submenuTable.Help.ShortHelpView(m.submenuKeys.ShortHelp()) +  "\n"
    }
    // Only show the main table when submenu is not active
    return baseStyle.Render(m.table.View()) + "\n" + 
	m.table.HelpView() + "\n" + 
	m.table.Help.ShortHelpView(m.tableKeys.ShortHelp()) + "\n"
}

func  createTable() model{

	columns := []table.Column{
		{Title: "Error:", Width: 30},
		{Title: "Count:", Width: 10},			
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

	m := model{t, additionalKeyMap(), table2, false, subMenuKeyMap(), map[string]int{}, map[string]int{} ,^uint64(0)}
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
    var m model = createTable()
    quit := make(chan struct{})

    p := tea.NewProgram(m)
    go func() {
        if _,err := p.Run(); err != nil {
            fmt.Println("Error running TUI:", err)
        }
        close(quit)
    }()

    go can_listener(p)
    <-quit
}

