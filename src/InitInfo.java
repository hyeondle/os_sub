public class InitInfo {
    private int CONTEXT_SWITCH;
    private int QUANTUM;
    private int RQ_SIZE;

    public InitInfo(int CONTEXT_SWITCH, int QUANTUM, int RQ_SIZE) {
        this.CONTEXT_SWITCH = CONTEXT_SWITCH;
        this.QUANTUM = QUANTUM;
        this.RQ_SIZE = RQ_SIZE;
    }

    public int getCONTEXT_SWITCH() {
        return CONTEXT_SWITCH;
    }

    public int getQUANTUM() {
        return QUANTUM;
    }

    public int getRQ_SIZE() {
        return RQ_SIZE;
    }
}