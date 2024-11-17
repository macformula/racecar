package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"math"
	"net"
	"sort"
	"strconv"
	"time"

	box "main/components"
	table "main/components"

	"github.com/charmbracelet/bubbles/key"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
	"golang.org/x/sys/unix"
)


const canFrameSize = 16      // CAN frame size (for a standard CAN frame)

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

func RowValuesToRow(r RowValues) table.Row{
	return table.Row{r.Error, strconv.Itoa(r.Count), strconv.Itoa(r.Recency)}
}

func SortRowValuesBy(r []RowValues, column int) []RowValues {
	// Create a deep copy of the input slice
	copied := make([]RowValues, len(r))
	copy(copied, r)

	// Sort the copied slice
	sort.SliceStable(copied, func(i, j int) bool {
		switch column {
		case 0:
			return copied[i].Error < copied[j].Error
		case 1:
			return copied[i].Count < copied[j].Count
		case 2:
			return copied[i].Recency < copied[j].Recency
		}
		return false
	})

	return copied
}

func toTableRows(r []RowValues) []table.Row {
	var rows []table.Row
	for _, v := range r {
		// Check if the RowValues is "non-empty" and Active
		if v.Error != "" && v.Count != 0 && v.Active{
			rows = append(rows, RowValuesToRow(v))
		}
	}
	return rows
}

func (m model) getDescription(errorName string) string {
	for i := range(len(m.rowsValues)){
		if m.rowsValues[i].Error == errorName{
			return m.rowsValues[i].Description
		}
	}
	return "No description available"
}

func findError(errorName string, rowValues []RowValues) int {
	for i := range(len(rowValues)){
		if rowValues[i].Error == errorName{
			return i
		}
	}
	return -1
}
func findErrorAfterSort(errorName string, rowValues []RowValues) int {
	cnt := 0
	for i := range(len(rowValues)){
		if rowValues[i].Error != ""{
			cnt += 1
		} 
		if rowValues[i].Error == errorName{
			return cnt - 1
		}
	}
	return -1
}

func (m model) resetRowValue(errorName string) {
	for i := range(len(m.rowsValues)){
		if m.rowsValues[i].Error == errorName{
			m.rowsValues[i].Count = 0
			m.rowsValues[i].Recency = 0
		}
	}
}


func (m model) getIgnoredRows() []table.Row {
	var rows []table.Row
	for i := range(len(m.rowsValues)){
		if !m.rowsValues[i].Active && m.rowsValues[i].Error != ""{
		rows = append(rows, table.Row{m.rowsValues[i].Error})
		}
	}
	return rows
}

type RowValues struct{
	Error string
	Count int
	Recency int
	Description string
	Active bool
}


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
	rowsValues  []RowValues
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

func tickEvery(t time.Duration) tea.Cmd {
	return func() tea.Msg {
		time.Sleep(t)
		return TickMsg{}
	}
}

// Check if the cursor is over bounds
func overBounds(table table.Model) bool{
	return len(table.Rows())==0 || table.Cursor() < 0 || table.Cursor() >= len(table.Rows()) 
}

func (m *model) refresh(){
	rows := m.rowsValues
	for i := range(len(rows)){
		if m.rowsValues[i].Count !=0{
			m.rowsValues[i].Recency += 1
		}
	}
}

func (m model) Init() tea.Cmd { return tickEvery(m.t) }

// Table and ignore subtable update loop.
func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd
	switch msg := msg.(type) {
	case tea.KeyMsg:
		if m.submenuActive{
			// If submenu is active, only let the submenu handle KeyMsg
			switch {
			case key.Matches(msg, table.DefaultKeyMap().LineUp):
				m.submenuTable.MoveUp(1)			
				return m,nil	
			case key.Matches(msg, table.DefaultKeyMap().LineDown):
				m.submenuTable.MoveDown(1)		
				return m,nil			
			case key.Matches(msg, m.submenuKeys.i):
				if m.submenuTable.SelectedRow() == nil{
					return m, nil
				}
				
				// Toggle the mask for the selected error and delete the row. 
				errorIndex := findError(m.submenuTable.SelectedRow()[0], m.rowsValues)
				m.ignoreMask = toggleBit(m.ignoreMask, errorIndex)
				m.rowsValues[errorIndex].Active = true
				newRows := m.getIgnoredRows()
				m.submenuTable.SetRows(newRows)

				// May be unnecessary, but just in case we update the cursor.
				if m.submenuTable.Cursor() == 0{
					m.submenuTable.MoveDown(1)
				}else{
					m.submenuTable.MoveUp(1)
				}

			case key.Matches(msg, m.submenuKeys.s):
				m.submenuActive = false  // Hide submenu
				m.table.Focus()  // Refocus the main table
				m.submenuTable.Blur() // Unfocus the submenu
				m.table.SetShowCursor(true) // Reshow the cursor on the main table

				return m, nil
	
			case key.Matches(msg, m.tableKeys.q):
				return m, tea.Quit
			}

			m.submenuTable, cmd = m.submenuTable.Update(msg)
			return m, cmd
		}


		switch {
		case key.Matches(msg, m.tableKeys.q):
			return m, tea.Quit
		case key.Matches(msg, m.tableKeys.a):	
			
			// Prevent panic if no row is selected
			if m.table.SelectedRow() == nil{
				m.table.SetCursorAndViewport(0)
				return m, nil
			}

			// Remove the row and reset its count.
			errorIndex := findError(m.table.SelectedRow()[0], m.rowsValues)
			m.rowsValues[errorIndex].Count = 0
			newRows := toTableRows(m.rowsValues)
			m.table.SetRows(newRows)


			// May be unnecessary, but just in case we update the cursor.
			if m.table.Cursor() == 0{
				m.table.MoveDown(1)
			}else{
				m.table.MoveUp(1)
			}
			return m, nil
		case key.Matches(msg, m.tableKeys.i):

			// Prevent panic if no row is selected
			if m.table.SelectedRow() == nil{
				m.table.SetCursorAndViewport(0)
				return m, nil
			}

			// Toggle the mask for the selected error and move it to the ignore menu.
			errorIndex := findError(m.table.SelectedRow()[0], m.rowsValues)
			m.ignoreMask = toggleBit(m.ignoreMask, errorIndex)
			m.rowsValues[errorIndex].Active = false

			// Remove the row from the main table and reset its count.
			m.rowsValues[errorIndex].Count = 0
			newRows := toTableRows(m.rowsValues)
			m.table.SetRows(newRows)

			//Set add the ignored rows to submenu
			ignoredRows := m.getIgnoredRows()
			m.submenuTable.SetRows(ignoredRows)
			// May be unnecessary, but just in case we update the cursor.
			m.table.MoveDown(1)

			return m, nil
        case key.Matches(msg, m.tableKeys.s):
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
		case key.Matches(msg, table.DefaultKeyMap().LineUp):
			m.table.MoveUp(1)
			return m,nil	
		case key.Matches(msg, table.DefaultKeyMap().LineDown):
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
		m.refresh()
		for k := 0; k < 64; k++ { // Iterate through all 64 bits
			if msg.Value&(1<<k) != 0 { // Check if the k-th bit is set
				var errorNumberStr string = strconv.Itoa(k)
				if m.rowsValues[k].Error == ""{
					m.rowsValues[k] = RowValues{Error: "error" + errorNumberStr, Count: 1, Recency: 0, Description: "No description available for Error" + errorNumberStr, Active: true}
				} else{
					m.rowsValues[k].Count += 1
					m.rowsValues[k].Recency = 0
				}
			}

			// Sort the table by the recency.
			currentError := m.table.SelectedRow()
			newRowValues :=  SortRowValuesBy(m.rowsValues,2)
			newRows := toTableRows(newRowValues)
			m.table.SetRows(newRows)

			// Fix the cursor back onto the row it was on before the sort.
			if currentError != nil{
				newIndex := findErrorAfterSort(currentError[0], newRowValues)
				m.table.SetCursorAndViewport(newIndex)
			}
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


// key bindings for the main table
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
			key.WithKeys("q", "ctrl+c"),
			key.WithHelp("q/ctrl+c", "quit"),
		),
	}
}

// key bindings for the submenu
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
		m.box.SetText(m.submenuTable.SelectedRow()[0], m.getDescription(m.submenuTable.SelectedRow()[0]))
		out = out + m.box.View()
	} else if m.table.SelectedRow() != nil && !m.submenuActive{
		m.box.SetText(m.table.SelectedRow()[0], m.getDescription(m.table.SelectedRow()[0]))
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

		make([]RowValues, 64),
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


func can_listener(p *tea.Program, canInterface string) {
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
	warnFlag := flag.Int("w", 5, "warning time for the table (integer value)")
	canInterfaceFlag := flag.String("i", "vcan0", "CAN interface to listen on")

	flag.Parse()
	
	// Initialize the model and run the program
	var m model = initViewer(*warnFlag)
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
	go can_listener(p, *canInterfaceFlag)


	<-quit
}
