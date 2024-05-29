class ProcessInfo {
    private int time;
    private String action;

    public ProcessInfo(int time, String action) {
        this.time = time;
        this.action = action;
    }

    public int getTime() {
        return time;
    }

    public String getAction() {
        return action;
    }

    @Override
    public String toString() {
        return time + "s : " + action;
    }
}