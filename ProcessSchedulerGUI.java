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

    public ProcessSchedulerGUI() {
        setTitle("Process Scheduler");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(800, 400);
        setLocationRelativeTo(null);

        headerLine = "Process ID Burst Time Arrival Time Priority";

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

        processIdField = new JTextField(5);
        burstTimeField = new JTextField(5);
        arrivalTimeField = new JTextField(5);
        priorityField = new JTextField(5);


        JPanel inputPanel = new JPanel();
        inputPanel.add(new JLabel("Process ID:"));
        inputPanel.add(processIdField);
        inputPanel.add(new JLabel("Burst Time:"));
        inputPanel.add(burstTimeField);
        inputPanel.add(new JLabel("Arrival Time:"));
        inputPanel.add(arrivalTimeField);
        inputPanel.add(new JLabel("Priority:"));
        inputPanel.add(priorityField);
        inputPanel.add(addButton);

        JPanel panel = new JPanel();
        panel.add(loadButton);
        panel.add(saveButton);
        panel.add(runButton);

        add(scrollPane, BorderLayout.CENTER);
        add(panel, BorderLayout.SOUTH);
        add(inputPanel, BorderLayout.NORTH);
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

    private void runScheduler() {
        JFrame newFrame = new JFrame("Choose Priority");
        newFrame.setSize(300, 200);
        newFrame.setLayout(new GridLayout(1, 4));

        String[] buttonNames = {"fcfs", "sjf", "srtf", "rr"};

        for (int i = 0; i < 4; i++) {
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
                String currentWorkingDirectory = System.getProperty("user.dir");
                ProcessBuilder makeProcessBuilder = new ProcessBuilder("make", "scheduler");
                makeProcessBuilder.inheritIO();
                Process makeProcess = makeProcessBuilder.start();
                int makeExitCode = makeProcess.waitFor();
                if (makeExitCode != 0) {
                    System.out.println("Make Failed : " + makeExitCode);
                    return;
                }

                // String currentWorkingDirectory = System.getProperty("user.dir");
                // String command = "./bins/scheduler " + tempFile.getAbsolutePath() + " " + priority;

                // ProcessBuilder processBuilder = new ProcessBuilder(command.split(" "));
                // runProcessBuilder.inheritIO();
                // Process runProcess = runProcessBuilder.start();
                // int runExitCode = runProcess.waitFor();
                // if (runExitCode != 0) {
                //     System.out.println("Run Failed : " + runExitCode);
                //     return;
                // }
                
                // 현재 작업 디렉토리의 경로를 얻습니다.
                // String currentWorkingDirectory = System.getProperty("user.dir");
                String command = "./bins/scheduler " + tempFile.getAbsolutePath() + " " + priority;

                ProcessBuilder processBuilder = new ProcessBuilder(command.split(" "));
                // ProcessBuilder의 작업 디렉토리를 현재 작업 디렉토리로 설정합니다.
                processBuilder.directory(new File(currentWorkingDirectory));
                processBuilder.redirectErrorStream(true);

                java.lang.Process process = processBuilder.start();

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
                    showOutput(output.toString());
                });

            } catch (IOException | InterruptedException ex) {
                ex.printStackTrace();
            }
        }).start();
    }

    private void showOutput(String output) {
        JFrame outputFrame = new JFrame("Scheduler Output");
        outputFrame.setSize(1000, 600);

        JPanel panel = new JPanel(new BorderLayout());

        JTextArea textArea = new JTextArea(output);
        textArea.setEditable(false);
        JScrollPane scrollPane = new JScrollPane(textArea);

        panel.add(scrollPane, BorderLayout.CENTER);

        String[] lines = output.split("\n");
        HashMap<Integer, ArrayList<ProcessInfo>> processMap = new HashMap<>();
        int maxTime = 0;

        Pattern pattern = Pattern.compile("(\\d+)s : (Process|Monitor :) (\\d+) is (submitted|running|working|waiting)");
        Pattern pattern1 = Pattern.compile("(\\d+)s : (Monitor :) (\\d+) finished");
        for(String line : lines) {
            Matcher matcher = pattern.matcher(line);
            Matcher matcher1 = pattern1.matcher(line);
            if (matcher.find()) {
                int time = Integer.parseInt(matcher.group(1));
                int processId = Integer.parseInt(matcher.group(3));
                String action = matcher.group(4);

                processMap.putIfAbsent(processId, new ArrayList<>());
                processMap.get(processId).add(new ProcessInfo(time, action));
                if (time > maxTime) maxTime = time;
            }
            if (matcher1.find()) {
                int time1 = Integer.parseInt(matcher1.group(1));
                int processId1 = Integer.parseInt(matcher1.group(3));

                processMap.putIfAbsent(processId1, new ArrayList<>());
                processMap.get(processId1).add(new ProcessInfo(time1, "finished"));
                if (time1 > maxTime) maxTime = time1;
            }
        }

        // 결과 출력
//        processMap.forEach((id, list) -> {
//            System.out.println("Process " + id + ":");
//            list.forEach(processInfo -> System.out.println(processInfo.toString()));
//            System.out.println();
//        });

        GanttChart ganttChart = new GanttChart(processMap, maxTime);
        JScrollPane chartScrollPane = new JScrollPane(ganttChart, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);


        panel.add(chartScrollPane, BorderLayout.SOUTH);
        outputFrame.add(panel);
        outputFrame.setVisible(true);
    }


    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            ProcessSchedulerGUI gui = new ProcessSchedulerGUI();
            gui.setVisible(true);
        });
    }
}
