추가해야 할 것

자바에서 Makefile 실행해서 Context_switch, Quantum 설정하게 하느냐
혹은 여기서 인자로 입력받느냐로 결정.

Makefile의 make 로 빠를 수 있음.

Makefile 제작,
QUANTUM ?= 5 과 같은 구문을 통해 기본값, 
make QUANTUM=20 과 같이 전달 가능.

자바의 경우, ProcessBuilder makeProcessBuilder = new ProcessBuilder("make" + 매크로 선언);
를 통해 make가 가능, 인자 전달하고 make 완료 대기 후 실행하도록 함
int makeExitCode = makeProcess.waitFor();
로 대기, 다시
ProcessBuilder runProcessBuilder = new ProcessBuilder("./a.out" + 인자);
처럼 가능
