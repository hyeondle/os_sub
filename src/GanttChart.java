import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class GanttChart extends JPanel {
    private final HashMap<Integer, ArrayList<ProcessInfo>> processMap;
    private final int maxTime;
    private final ArrayList<ResultInfo> resultInfos;
    private final int cellWidth;
    private final int processHeight;
    private final int processGap;
    private final JPanel[][] cells;


    public GanttChart(HashMap<Integer, ArrayList<ProcessInfo>> processMap, int maxTime, ArrayList<ResultInfo> resultInfos) {
        this.processMap = processMap;
        this.maxTime = maxTime;
        this.resultInfos = resultInfos;
        this.cellWidth = 5;
        this.processHeight = 20;
        this.processGap = 20;
        this.cells = new JPanel[processMap.size()][maxTime];

        setLayout(null);
        this.setPreferredSize(new Dimension((maxTime * 2) * cellWidth, processMap.size() * (processHeight + processGap) + 100));
        initializeCells();
    }

    private void initializeCells() {
        int y = processGap + 70;
        int processIndex = 0;
        for (Map.Entry<Integer, ArrayList<ProcessInfo>> entry : processMap.entrySet()) {
            int x = 50;
            JLabel idxLabel = new JLabel(String.valueOf(processIndex + 1));
            idxLabel.setBounds(x - 30, y, 20, processHeight); // 레이블의 위치와 크기를 설정
            this.add(idxLabel); // 레이블을 패널에 추가

            for (int time = 0; time < maxTime; time++) {
                cells[processIndex][time] = new JPanel();
                cells[processIndex][time].setBackground(Color.WHITE);
                cells[processIndex][time].setBorder(BorderFactory.createLineBorder(Color.BLACK));
                cells[processIndex][time].setBounds(x, y, cellWidth, processHeight);
                add(cells[processIndex][time]);
                x += cellWidth;
            }
            y += processHeight + processGap;
            processIndex++;
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        drawTitle(g);
        drawLegend(g);
        updateCells();
    }

    private void drawTitle(Graphics g) {
        g.setFont(new Font("Arial", Font.BOLD, 20));
        g.drawString("Gantt Chart", 50, 30);
        g.setFont(new Font("Arial", Font.PLAIN, 12));
    }

    private void drawLegend(Graphics g) {
        int x = 50;
        int y = 50;
        int legendWidth = 50;
        int legendHeight = 20;

        String[] actions = {"working", "waiting", "empty | context_switch"};
        Color[] colors = {Color.BLUE, Color.ORANGE, Color.WHITE};

        for (int i = 0; i < actions.length; i++) {
            g.setColor(colors[i]);
            g.fillRect(x, y, legendWidth, legendHeight);
            g.setColor(Color.BLACK);
            g.drawRect(x, y, legendWidth, legendHeight);
            g.drawString(actions[i], x + legendWidth + 5, y + 15);
            x += legendWidth + 80; // 간격 조절
        }
    }

    private void updateCells() {
        int processIndex = 0;
        for (Map.Entry<Integer, ArrayList<ProcessInfo>> entry : processMap.entrySet()) {
            //int processId = entry.getKey();
            ArrayList<ProcessInfo> infoList = entry.getValue();

            for (ProcessInfo info : infoList) {
                int time = info.getTime();
                if (time > 0 && time < maxTime) {
                    Color color = getColorForAction(info.getAction());
                    cells[processIndex][time-1].setBackground(color);
                }
            }
            processIndex++;
        }
    }

    private Color getColorForAction(String action) {
        switch (action) {
            case "working", "running":
                return Color.BLUE;
            case "waiting":
                return Color.ORANGE;
            default:
                return Color.WHITE;
        }
    }
}
