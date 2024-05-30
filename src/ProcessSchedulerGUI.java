import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.io.*;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ProcessSchedulerGUI extends JFrame {
    private DefaultTableModel tableModel;
    private JTable table;
    private String headerLine;
    private JTextField processIdField, burstTimeField, arrivalTimeField, priorityField;
    private JTextField contextSwitchField, quantumField, rqSizeField;
    private String contextSwitch = "0", quantum = "20", rqSize = "3";

    public ProcessSchedulerGUI() {
            setTitle("Process Scheduler");
            setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            setSize(800, 400);
            setLocationRelativeTo(null);

            tableModel = new DefaultTableModel(new Object[]{"Process ID", "Burst Time", "Arrival Time", "Priority"}, 0);
            table = new JTable(tableModel);
            JScrollPane scrollPane = new JScrollPane(table);

            JButton loadButton = new JButton("Load File");
            loadButton.addActionListener(e -> loadFile());

            JButton saveButton = new JButton("Save File");
            saveButton.addActionListener(e -> saveFile());

            JButton runButton = new JButton("Run Scheduler");
            runButton.addActionListener(e -> runScheduler());

            JButton addButton = new JButton("Add");
            addButton.addActionListener(e -> addRow());

            JButton setButton = new JButton("Set");
            setButton.addActionListener(e -> setInit());

            JButton randomButton = new JButton("Random");
            randomButton.addActionListener(e -> addRandomRow());

            processIdField = new JTextField(5);
            burstTimeField = new JTextField(5);
            arrivalTimeField = new JTextField(5);
            priorityField = new JTextField(5);

            contextSwitchField = new JTextField(5);
            quantumField = new JTextField(5);
            rqSizeField = new JTextField(5);

            JPanel inputPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 5, 5));
            inputPanel.add(new JLabel("Process ID:"));
            inputPanel.add(processIdField);
            inputPanel.add(new JLabel("Burst Time:"));
            inputPanel.add(burstTimeField);
            inputPanel.add(new JLabel("Arrival Time:"));
            inputPanel.add(arrivalTimeField);
            inputPanel.add(new JLabel("Priority:"));
            inputPanel.add(priorityField);
            inputPanel.add(addButton);
            inputPanel.add(randomButton);

            JPanel initPanel = new JPanel(new GridBagLayout());
            GridBagConstraints gbc = new GridBagConstraints();

            gbc.gridx = 0; gbc.gridy = 0;
            initPanel.add(new JLabel("CONTEXT_SWITCH: "), gbc);
            gbc.gridx = 1; gbc.gridy = 0;
            initPanel.add(contextSwitchField, gbc);

            gbc.gridx = 0; gbc.gridy = 1;
            initPanel.add(new JLabel("QUANTUM: "), gbc);
            gbc.gridx = 1; gbc.gridy = 1;
            initPanel.add(quantumField, gbc);

            gbc.gridx = 0; gbc.gridy = 2;
            initPanel.add(new JLabel("RQ_SIZE: "), gbc);
            gbc.gridx = 1; gbc.gridy = 2;
            initPanel.add(rqSizeField, gbc);

            gbc.gridx = 1; gbc.gridy = 3;
            initPanel.add(setButton, gbc);

            JPanel buttonPanel = new JPanel();
            buttonPanel.add(loadButton);
            buttonPanel.add(saveButton);
            buttonPanel.add(runButton);

            setLayout(new BorderLayout());
            add(scrollPane, BorderLayout.CENTER);
            add(buttonPanel, BorderLayout.SOUTH);
            add(inputPanel, BorderLayout.NORTH);
            add(initPanel, BorderLayout.WEST);
    }

        private void loadFile() {
            JFileChooser fileChooser = new JFileChooser();
            int result = fileChooser.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File loadedFile = fileChooser.getSelectedFile();
                try (BufferedReader br = new BufferedReader(new FileReader(loadedFile))) {
                    headerLine = br.readLine();
                    String line;
                    tableModel.setRowCount(0);
                    while ((line = br.readLine()) != null) {
                        String[] data = line.split(" ");
                        tableModel.addRow(data);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        private void saveFile() {
            JFileChooser fileChooser = new JFileChooser();
            int result = fileChooser.showSaveDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File file = fileChooser.getSelectedFile();
                try (BufferedWriter bw = new BufferedWriter(new FileWriter(file))) {
                    bw.write(headerLine);
                    bw.newLine();
                    for (int i = 0; i < tableModel.getRowCount(); i++) {
                        for (int j = 0; j < tableModel.getColumnCount(); j++) {
                            bw.write(Objects.requireNonNull(tableModel.getValueAt(i, j)).toString() + " ");
                        }
                        bw.newLine();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        private void setInit() {
            contextSwitch = contextSwitchField.getText().isEmpty() ? "0" : contextSwitchField.getText();
            quantum = quantumField.getText().isEmpty() ? "20" : quantumField.getText();
            rqSize = rqSizeField.getText().isEmpty() ? "3" : rqSizeField.getText();
        }

        private void addRow() {
            String processId = processIdField.getText();
            String burstTime = burstTimeField.getText();
            String arrivalTime = arrivalTimeField.getText();
            String priority = priorityField.getText();

            if (!processId.isEmpty() && !burstTime.isEmpty() && !arrivalTime.isEmpty() && !priority.isEmpty()) {
                tableModel.addRow(new Object[]{processId, burstTime, arrivalTime, priority});
                processIdField.setText("");
                burstTimeField.setText("");
                arrivalTimeField.setText("");
                priorityField.setText("");
            } else {
                JOptionPane.showMessageDialog(this, "모든 필드를 채워주세요.", "입력 오류", JOptionPane.ERROR_MESSAGE);
            }
        }

        private void addRandomRow() {
            Random random = new Random();

            int lastRowIndex = tableModel.getRowCount() - 1;
            int processId = lastRowIndex >= 0 ? Integer.parseInt(tableModel.getValueAt(lastRowIndex, 0).toString()) + 1 : 1;
            int burstTime = random.nextInt(10) + 1;
            int arrivalTime = random.nextInt(20);
            int priority = random.nextInt(10) + 1;

            tableModel.addRow(new Object[]{String.valueOf(processId), String.valueOf(burstTime), String.valueOf(arrivalTime), String.valueOf(priority)});
        }

        private void runScheduler() {
            JFrame newFrame = new JFrame("Choose Priority");
            newFrame.setSize(300, 200);
            newFrame.setLayout(new GridLayout(1, 5));

            String[] buttonNames = {"fcfs", "sjf", "srtf", "rr", "lrrwp"};

            for (int i = 0; i < 5; i++) {
                JButton button = new JButton(buttonNames[i]);
                int finalI = i + 1;  // executeCommand에 전달될 값 조정
                button.addActionListener(e -> {
                    executeCommand(finalI);
                    newFrame.dispose();
                });
                newFrame.add(button);
            }

            newFrame.setLocationRelativeTo(this);
            newFrame.setVisible(true);
        }


    private void executeCommand(int priority) {
        new Thread(() -> {
            try {
                // 테이블 데이터를 임시 파일에 저장
                File tempFile = File.createTempFile("process_data", ".txt");
                try (BufferedWriter bw = new BufferedWriter(new FileWriter(tempFile))) {
                    for (int i = 0; i < tableModel.getRowCount(); i++) {
                        for (int j = 0; j < tableModel.getColumnCount(); j++) {
                            bw.write(Objects.requireNonNull(tableModel.getValueAt(i, j)).toString() + " ");
                        }
                        bw.newLine();
                    }
                }

                // 현재 작업 디렉토리의 경로를 얻습니다.
                String currentWorkingDirectory = System.getProperty("user.dir");
                System.out.println(contextSwitch + " " + quantum + " " + rqSize);
                ProcessBuilder makeProcessBuilder = new ProcessBuilder("make", "scheduler",
                        "CONTEXT_SWITCH=" + contextSwitch,
                        "QUANTUM=" + quantum,
                        "RQ_SIZE=" + rqSize);
                makeProcessBuilder.inheritIO();
                Process makeProcess = makeProcessBuilder.start();
                int makeExitCode = makeProcess.waitFor();
                if (makeExitCode != 0) {
                    System.out.println("Make Failed : " + makeExitCode);
                    return;
                }

                String command = "./bins/scheduler " + tempFile.getAbsolutePath() + " " + priority;

                ProcessBuilder processBuilder = new ProcessBuilder(command.split(" "));
                // ProcessBuilder의 작업 디렉토리를 현재 작업 디렉토리로 설정합니다.
                processBuilder.directory(new File(currentWorkingDirectory));
                processBuilder.redirectErrorStream(true);

                Process process = processBuilder.start();

                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                StringBuilder output = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    String finalLine = line;
                    SwingUtilities.invokeLater(() -> {
                        output.append(finalLine).append("\n");
                    });
                }
                // 프로세스가 종료될 때까지 대기
                int exitVal = process.waitFor();
                SwingUtilities.invokeLater(() -> {
                    if (exitVal == 0) {
                        System.out.println("실행 성공.");
                    } else {
                        // 오류 처리
                        System.out.println("실행 실패.");
                    }
                    try {
                        showOutput(output.toString());
                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                });

                ProcessBuilder exitProcessBuilder = new ProcessBuilder("make", "clean_scheduler");
                exitProcessBuilder.inheritIO();
                Process exitProcess = exitProcessBuilder.start();
                int exitExitCode = exitProcess.waitFor();
                if (exitExitCode != 0) {
                    System.out.println("Exit Failed : " + exitExitCode);
                    return;
                }
            } catch (IOException | InterruptedException ex) {
                ex.printStackTrace();
            }
        }).start();
    }

    private void showOutput(String output) throws IOException {
        JFrame outputFrame = new JFrame("Scheduler Output");
        outputFrame.setSize(1200, 800);

        // BorderLayout의 hgap과 vgap을 0으로 설정
        JPanel panel = new JPanel(new BorderLayout(0, 0));

        JTextArea textArea = new JTextArea(output);
        textArea.setEditable(false);
        JScrollPane textScrollPane = new JScrollPane(textArea);
        textScrollPane.setViewportBorder(null); // 내부 여백 제거

        JPanel resultPanel = new JPanel(new BorderLayout(0, 0));

        String[] lines = output.split("\n");
        HashMap<Integer, ArrayList<ProcessInfo>> processMap = new HashMap<>();
        ArrayList<ResultInfo> result = new ArrayList<>();
        HashMap<Integer, ArrayList<ProcessInfo>> processResult = new HashMap<>();
        int maxTime = 0;

        Pattern pattern = Pattern.compile("(\\d+)s : (Process) (\\d+) is (submitted|running|waiting)");
        Pattern monitorPattern = Pattern.compile("(\\d+)s : (Monitor) : (\\d+) is (working|finished)");
        Pattern processPattern = Pattern.compile("Process_ (\\d+) (Response|Waiting|Turnaround) Time (:|;) (\\d+)");
        Pattern resultPattern = Pattern.compile("Average (Response|Waiting|Turnaround) Time: (\\d+\\.\\d+)");

        for (String line : lines) {
            Matcher matcher = pattern.matcher(line);
            Matcher monitorMatcher = monitorPattern.matcher(line);
            Matcher processMatcher = processPattern.matcher(line);
            Matcher resultMatcher = resultPattern.matcher(line);

            if (matcher.find()) {
                int time = Integer.parseInt(matcher.group(1));
                int processId = Integer.parseInt(matcher.group(3));
                String action = matcher.group(4);

                processMap.putIfAbsent(processId, new ArrayList<>());
                processMap.get(processId).add(new ProcessInfo(time, action));
                if (time > maxTime) maxTime = time;
            }
            if (monitorMatcher.find()) {
                int time1 = Integer.parseInt(monitorMatcher.group(1));
                int processId = Integer.parseInt(monitorMatcher.group(3));
                String action = monitorMatcher.group(4);

                processMap.putIfAbsent(processId, new ArrayList<>());
                processMap.get(processId).add(new ProcessInfo(time1, action));
                if (time1 > maxTime) maxTime = time1;
            }
            if (processMatcher.find()) {
                int id = Integer.parseInt(processMatcher.group(1));
                String action2 = processMatcher.group(2);
                int time2 = Integer.parseInt(processMatcher.group(4));

                processResult.putIfAbsent(id, new ArrayList<>());
                processResult.get(id).add(new ProcessInfo(time2, action2));
            }
            if (resultMatcher.find()) {
                double time3 = Double.parseDouble(resultMatcher.group(2));
                result.add(new ResultInfo(time3));
            }
        }

        JPanel textPanel = new JPanel(new BorderLayout(0, 0));
        textPanel.add(textScrollPane, BorderLayout.CENTER);
        textPanel.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));

        JPanel resultChartPanel = new JPanel(new BorderLayout(0, 0));
        resultChartPanel.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));

        JScrollPane resultContentPanel = createResultPanel(processResult, result);
        resultPanel.add(resultContentPanel, BorderLayout.CENTER);

        GanttChart ganttChart = new GanttChart(processMap, maxTime, result);
        JScrollPane chartScrollPane = new JScrollPane(ganttChart, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        chartScrollPane.setBorder(BorderFactory.createEmptyBorder(0,0,0,0));
        chartScrollPane.setViewportBorder(null); // 내부 여백 제거

        resultChartPanel.add(resultPanel, BorderLayout.CENTER);

        panel.add(textPanel, BorderLayout.WEST);
        panel.add(resultChartPanel, BorderLayout.EAST);
        panel.add(chartScrollPane, BorderLayout.CENTER);

        textPanel.setPreferredSize(new Dimension(250,0));
        resultChartPanel.setPreferredSize(new Dimension(300,0));
        outputFrame.add(panel);
        outputFrame.setVisible(true);
    }

    private JScrollPane createResultPanel(HashMap<Integer, ArrayList<ProcessInfo>> processResult, ArrayList<ResultInfo> result) throws IOException {
        saveResultToFile(processResult, result);

        // 스크롤을 위해 내부 패널 생성
        JPanel resultPanel = new JPanel(new BorderLayout(0,0));
        resultPanel.setLayout(new BoxLayout(resultPanel, BoxLayout.Y_AXIS));
        resultPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));

        JLabel allLabel = new JLabel("Total Average Times");
        allLabel.setHorizontalAlignment(JLabel.CENTER);
        allLabel.setFont(new Font("Serif", Font.BOLD, 20));
        allLabel.setForeground(Color.RED);
        resultPanel.add(allLabel);

        JLabel aWLabel = new JLabel("Average Waiting Time: " + result.get(1).getTime());
        aWLabel.setHorizontalAlignment(JLabel.CENTER);
        aWLabel.setFont(new Font("Serif", Font.PLAIN, 16));
        resultPanel.add(Box.createVerticalStrut(10));
        resultPanel.add(aWLabel);

        JLabel aTLabel = new JLabel("Average Turnaround Time: " + result.get(2).getTime());
        resultPanel.add(Box.createVerticalStrut(10));
        aTLabel.setHorizontalAlignment(JLabel.CENTER);
        aTLabel.setFont(new Font("Serif", Font.PLAIN, 16));
        resultPanel.add(aTLabel);

        JLabel aRLabel = new JLabel("Average Response Time: " + result.get(0).getTime());
        aRLabel.setHorizontalAlignment(JLabel.CENTER);
        aRLabel.setFont(new Font("Serif", Font.PLAIN, 16));
        resultPanel.add(Box.createVerticalStrut(10));
        resultPanel.add(aRLabel);

        resultPanel.add(Box.createVerticalStrut(10));


        JLabel titleLabel = new JLabel("Average Process Times");
        titleLabel.setHorizontalAlignment(JLabel.CENTER);
        titleLabel.setFont(new Font("Serif", Font.BOLD, 20));
        titleLabel.setForeground(Color.BLUE);
        resultPanel.add(titleLabel);

        for (Map.Entry<Integer, ArrayList<ProcessInfo>> entry : processResult.entrySet()) {
            int id = entry.getKey();
            ArrayList<ProcessInfo> pr = entry.getValue();

            JLabel processLabel = new JLabel("Process " + id + ":");
            processLabel.setHorizontalAlignment(JLabel.CENTER);
            processLabel.setFont(new Font("Serif", Font.BOLD, 18));
            resultPanel.add(Box.createVerticalStrut(10));
            resultPanel.add(processLabel);

            for (ProcessInfo t : pr) {
                JLabel timeLabel = new JLabel(t.getAction() + " Time: " + t.getTime());
                timeLabel.setHorizontalAlignment(JLabel.CENTER);
                timeLabel.setFont(new Font("Serif", Font.PLAIN, 16));
                resultPanel.add(timeLabel);
            }
            resultPanel.add(Box.createVerticalStrut(10));
        }

        // JScrollPane에 resultPanel을 추가
        JScrollPane scrollPane = new JScrollPane(resultPanel);
        scrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);

        // 스크롤 기능이 포함된 JScrollPane를 반환
        return scrollPane;
    }

    private void saveResultToFile(HashMap<Integer, ArrayList<ProcessInfo>> processResult, ArrayList<ResultInfo> result) throws IOException {
        // 임시 파일 생성
        String currentWorkingDirectory = System.getProperty("user.dir");

        File tempFile = new File(currentWorkingDirectory, "output" + (int)(Math.random()*1000) + ".csv");
        try (BufferedWriter bw = new BufferedWriter(new FileWriter(tempFile))) {
            // 평균 대기 시간, 평균 반환 시간, 평균 응답 시간 저장
            bw.write("W,T,R");
            bw.newLine();
            bw.write(result.get(1).getTime() + "," + result.get(2).getTime() + "," + result.get(0).getTime());
            bw.newLine();

            // 프로세스별 시간 데이터 저장
            for (Map.Entry<Integer, ArrayList<ProcessInfo>> entry : processResult.entrySet()) {
                ArrayList<ProcessInfo> pr = entry.getValue();
                for (ProcessInfo t : pr) {
                    bw.write(t.getTime() + ","); // 여기서는 각 프로세스 정보를 콤마로 구분하여 저장합니다.
                }
                bw.newLine();
            }
        }
        // 파일 경로 출력 또는 다른 처리
        System.out.println("결과가 저장된 파일: " + tempFile.getAbsolutePath());
    }


    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            ProcessSchedulerGUI gui = new ProcessSchedulerGUI();
            gui.setVisible(true);
        });
    }
}
