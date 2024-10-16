package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"net"
	
	"strconv"
	
	"golang.org/x/sys/unix"
	"github.com/charmbracelet/bubbles/table"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
)


const (
	canInterface = "vcan0"  // Use the virtual CAN interface
	canFrameSize = 16       // CAN frame size (for a standard CAN frame)
)

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
	table table.Model
	// hiddenRows []table.Row
	hiddenCounts map[string]int
}

func deleteElementRow(slice []table.Row, index int) []table.Row{
    return append(slice[:index], slice[index+1:]...)
}

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


func (m model) Init() tea.Cmd { return nil }

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	var cmd tea.Cmd
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "esc":
			if m.table.Focused() {
				m.table.Blur()
			} else {
				m.table.Focus()
			}
		case "q", "ctrl+c":
			return m, tea.Quit
		// case "a":
			
		// 	if len(m.table.Rows())==0{
		// 		return m, nil
		// 	}
		// 	if m.table.Cursor() < 0 || m.table.Cursor() >= len(m.table.Rows()) {
		// 		return m, nil
		// 	}

		// 	v := m.table.SelectedRow()[1]
		// 	num, err:= strconv.Atoi(v)
		// 	if err != nil{
		// 		tea.Printf("bug")
		// 	}
		// 	// if len(m.GetHiddenRows() )> 0{
		// 	// 	test = "hello world"
		// 	// }	else{
		// 	// 	test = "nope"
		// 	// }



		// 	// m.table.SelectedRow()[1] = strconv.Itoa(num + 1)
		// 	i := findRow(m.hiddenRows,m.table.SelectedRow())
		// 	m.hiddenRows[i][1] = strconv.Itoa(num + 1)
		// 	m.table.SetRows(m.hiddenRows)
		// 	m.table.UpdateViewport()
		// 	return m, tea.Batch(
		// 		tea.Printf("%v", m.hiddenRows),
		// 		tea.Printf("%s", strconv.Itoa(i)),
		// 	)

		// case "i":
		// 	if len(m.table.Rows()) == 0{
		// 		return m, nil
		// 	}
			
		// 	s := m.hiddenRows
			
		// 	m.table.SetRows(deleteElementRow(m.table.Rows(),m.table.Cursor()))
		// 	m.table.UpdateViewport()
		// 	return m, tea.Batch(
		// 		tea.Printf("previous ,%v", s),
		// 		tea.Printf("after, %v", m.hiddenRows),
		// 	)
		// case "r":
		// 	if len(m.table.Rows()) == 0{
		// 		return m, nil
		// 	}

		// 	indexOfDeletedRow := findRow(m.hiddenRows, m.table.SelectedRow())
		// 	newHiddenRows := deleteElementRow( m.hiddenRows, indexOfDeletedRow)
		// 	m.SetHiddenRows(newHiddenRows)

		// 	m.table.SetRows(m.hiddenRows)
		// 	m.table.UpdateViewport()
		// 	return m, nil
		}
	case CANMsg:
		for k := 0; k < 64; k++ { // Iterate through all 32 bits
			if msg.Value&(1<<k) != 0 { // Check if the k-th bit is set
				if val, ok := m.hiddenCounts["error" + strconv.Itoa(k)]; ok{
					m.hiddenCounts["error" + strconv.Itoa(k) ] = val + 1
					
				} else{
					m.hiddenCounts["error" + strconv.Itoa(k)] = 1
				}

				index := findRowString(m.table.Rows(),  "error" + strconv.Itoa(k))
				if index == -1{
					//need to check for acknowledgement later
					newRow := append(m.table.Rows(), table.Row{ "error" + strconv.Itoa(k), "1"})
					m.table.SetRows(newRow)
				} else{
					newRows := m.table.Rows()
					newRows[index][1] = strconv.Itoa(m.hiddenCounts[newRows[index][0]])
					m.table.SetRows(newRows)
				}

				m.table.UpdateViewport()
				fmt.Printf("error%d occurred\n", k)
			}
		}
		return m, nil
	}
	m.table, cmd = m.table.Update(msg)
	return m, cmd
}

func (m model) View() string {
	return baseStyle.Render(m.table.View()) + "\n  " + m.table.HelpView() + "\n"
}

// func (m *model) SetHiddenRows(rows []table.Row) {
//     m.hiddenRows = rows
// }

func  createTable() model{

	columns := []table.Column{
		{Title: "Error:", Width: 30},
		{Title: "Count", Width: 10},			
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

	m := model{t, map[string]int{}}
	return m
}




func listen(p *tea.Program) {
	// Create a raw CAN socket using the unix package
	var sock int;
	var err error;
	// var mask uint64 = ^uint64(0)
	
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
		data := bytesToUint64(buf[8:])  //& mask  
		

		msg := CANMsg{ ID : canID, Value: data}
		p.Send(msg)
		// CAN data bytes (8 bytes)
		//fmt.Printf("CAN ID: 0x%X, Data: %08b\n", canID, data)	
		// for k := 0; k < 64; k++ { // Iterate through all 32 bits
		// 	if data&(1<<k) != 0 { // Check if the k-th bit is set
		// 		fmt.Printf("error%d occurred\n", k)
		// 	}
		// }
	}

		// Print the CAN message ID and data
		//fmt.Printf("CAN ID: 0x%X, Data: %08b\n", canID, data)
}
func main() {
	
	var m model = createTable()
	p := tea.NewProgram(m)
	go p.Run()
	go listen(p)

	select {}
	// if _, err := tea.NewProgram(m).Run(); err != nil {
	// 	fmt.Println("Error running program:", err)
	// 	os.Exit(1)
	// }
}

