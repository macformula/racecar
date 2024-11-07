// component/box.go
package component

import (
	"strings"

	"github.com/charmbracelet/lipgloss"
)

type Box struct {
	Width       int
	Title       string
	Description string
}

func (b *Box) SetWidth(width int){
	b.Width = width
}
 
// SetText updates the Title and Description of the Box
func (b *Box) SetText(title, description string) {
	b.Title = title
	b.Description = description
}

func (b Box) View() string {
	borderStyle := lipgloss.NewStyle().Border(lipgloss.RoundedBorder()).Padding(1, 2)
	titleStyle := lipgloss.NewStyle().Bold(true).Underline(true).Align(lipgloss.Center).Width(b.Width)
	descriptionStyle := lipgloss.NewStyle().Align(lipgloss.Left).Width(b.Width - 4)

	titleContent := titleStyle.Render(b.Title) + "\n"
	descriptionContent := descriptionStyle.Render(b.wrapText(b.Description))

	content := lipgloss.JoinVertical(lipgloss.Left, titleContent, descriptionContent)

	boxHeight := 3 + strings.Count(content, "\n \n \n")
	return borderStyle.Width(b.Width).Height(boxHeight).Render(content)
}

func (b Box) wrapText(text string) string {
	var wrapped string
	maxLineLength := b.Width - 4

	for len(text) > 0 {
		if len(text) < maxLineLength {
			wrapped += text
			break
		}

		lineEnd := maxLineLength
		if idx := strings.LastIndex(text[:maxLineLength], " "); idx != -1 {
			lineEnd = idx
		}
		wrapped += text[:lineEnd] + "\n"
		text = text[lineEnd+1:]
	}
	return wrapped
}
