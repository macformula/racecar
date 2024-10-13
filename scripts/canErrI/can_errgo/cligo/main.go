package main

import (
	"fmt"
	"os"
    	"strconv"
	"github.com/charmbracelet/bubbles/table"
	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
)




var baseStyle = lipgloss.NewStyle().
	BorderStyle(lipgloss.NormalBorder()).
	BorderForeground(lipgloss.Color("240"))


type model struct {
	table table.Model
}

func deleteElementRow(slice []table.Row, index int) []table.Row{
    return append(slice[:index], slice[index+1:]...)
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
		case "a": 
			v := m.table.SelectedRow()[1]
			num, err:= strconv.Atoi(v)
			if err != nil{
				tea.Printf("bug")
			}
			
			m.table.SelectedRow()[1] = strconv.Itoa(num + 1)
			m.table.UpdateViewport()
			return m, nil
		case "i":
			if len(m.table.Rows()) == 0{
				return m, nil
			}

			m.table.SetRows(deleteElementRow(m.table.Rows(),m.table.Cursor()))
			m.table.UpdateViewport()
			return m, nil
	}}
	m.table, cmd = m.table.Update(msg)
	return m, cmd
}

func (m model) View() string {
	return baseStyle.Render(m.table.View()) + "\n  " + m.table.HelpView() + "\n"
}

func main() {
	columns := []table.Column{
		{Title: "Error:", Width: 10},
		{Title: "Count", Width: 10},
				
	}

	rows := []table.Row{{"error444322322324","20"},
			     {"error2", "1"}}

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

	m := model{t}
	if _, err := tea.NewProgram(m).Run(); err != nil {
		fmt.Println("Error running program:", err)
		os.Exit(1)
	}
}
