import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class GanttChart extends JPanel {
    private final HashMap<Integer, ArrayList<ProcessInfo>> processMap;
    private final int maxTime;
    private final int fixPixel;
    private final int processHeight;
    private final int processGap;


    public GanttChart(HashMap<Integer, ArrayList<ProcessInfo>> processMap, int maxTime) {
        this.processMap = processMap;
        this.maxTime = maxTime;
        this.fixPixel = 20;
        this.processHeight = 20;
        this.processGap = 20;
        this.setPreferredSize(new Dimension(1000, processMap.size() * 80));
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        drawTitle(g);
        drawLegend(g);

        int y = processGap + 70;
        for (Map.Entry<Integer, ArrayList<ProcessInfo>> entry : processMap.entrySet()) {
            int processId = entry.getKey();
            ArrayList<ProcessInfo> infoList = entry.getValue();
            int x = 50;

            g.drawString("Process " + processId, 5, y+processHeight);

            for (ProcessInfo info : infoList) {
                int width = (fixPixel);
                Color color = getColorForAction(info.getAction());
                g.setColor(color);
                g.fillRect(x+20,y,width,processHeight);
                g.setColor(Color.BLACK);
                g.drawRect(x+20,y,width,processHeight);
                x += width;
            }
            y += processHeight + processGap;
        }
    }

    private void drawTitle(Graphics g) {
        g.setFont(new Font("Arial", Font.BOLD, 20));
        g.drawString("Gantt Chart", getWidth() / 2 - 50, 30);
        g.setFont(new Font("Arial", Font.PLAIN, 12));
    }

    private void drawLegend(Graphics g) {
        int x = 50;
        int y = 50;
        int legendWidth = 50;
        int legendHeight = 20;

        String[] actions = {"submitted", "running", "working", "waiting", "finished"};
        Color[] colors = {Color.YELLOW, Color.GREEN, Color.BLUE, Color.ORANGE, Color.RED};

        for (int i = 0; i < actions.length; i++) {
            g.setColor(colors[i]);
            g.fillRect(x, y, legendWidth, legendHeight);
            g.setColor(Color.BLACK);
            g.drawRect(x, y, legendWidth, legendHeight);
            g.drawString(actions[i], x + legendWidth + 5, y + 15);
            x += legendWidth + 80; // 간격 조절
        }
    }

    private Color getColorForAction(String action) {
        switch (action) {
            case "submitted":
                return Color.YELLOW;
            case "running":
                return Color.GREEN;
            case "working":
                return Color.BLUE;
            case "waiting":
                return Color.ORANGE;
            case "finished":
                return Color.RED;
            default:
                return Color.GRAY;
        }
    }
}
