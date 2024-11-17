// Custom text-box component for rendering a box with a title and description, will dynamically wrap text to fit within the box width.
package components

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

// View renders the Box component
func (b Box) View() string {
	borderStyle := lipgloss.NewStyle().Border(lipgloss.RoundedBorder()).Padding(1, 2).BorderForeground(lipgloss.Color("57"))
	titleStyle := lipgloss.NewStyle().Bold(true).Underline(true).Align(lipgloss.Center).Width(b.Width).MaxWidth(b.Width)
	descriptionStyle := lipgloss.NewStyle().Align(lipgloss.Left).Width(b.Width).MaxWidth(b.Width)

	titleContent := titleStyle.Render(b.Title) + "\n"
	descriptionContent := descriptionStyle.Render(b.wrapText(b.Description))

	content := lipgloss.JoinVertical(lipgloss.Left, titleContent, descriptionContent)

	boxHeight := 3 + strings.Count(content, "\n \n \n")
	return borderStyle.Width(b.Width).Height(boxHeight).Render(content)
}

// Function to wrap text to fit within the box width
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
