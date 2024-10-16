#OS Scheduling Algorithms with Visualization

## Project Details

<details>
  <summary>Kor</summary>

## 멀티스레드를 활용한 CPU 스케줄링 알고리즘 구현 및 시각화  

## 📋 프로젝트 개요  
이 프로젝트는 운영체제의 대표적인 **CPU 스케줄링 알고리즘**을 C 언어로 구현하고, 각 알고리즘의 실행 과정을 **Java**를 이용해 시각화한 프로젝트입니다. 스케줄링 알고리즘의 이해를 돕고, 다양한 환경에서의 성능 차이를 확인할 수 있도록 설계되었습니다.  

### 주요 기능  
- **멀티스레드**를 활용한 스케줄링 알고리즘 구현  
- **FCFS**, **Round-Robin**, **SJF**, **SRTF** 스케줄링 알고리즘 지원  
- 자바 기반 GUI를 통한 **실시간 시각화**  
- 다양한 프로세스의 실행 및 대기 상황을 시각적으로 확인  

## 🛠️ 구현된 스케줄링 알고리즘  

### 1. FCFS (First-Come, First-Served)  
- **선입선처리 방식**으로, 가장 먼저 도착한 프로세스를 먼저 처리합니다.  
- 비선점(Non-preemptive) 방식으로, 실행 중인 프로세스가 종료될 때까지 다른 프로세스가 실행되지 않습니다.  
- **장점:** 구현이 간단하며 직관적입니다.  
- **단점:** 대기 시간이 길어지는 **Convoy Effect**가 발생할 수 있습니다.  

### 2. Round-Robin (RR)  
- 프로세스가 정해진 **타임 슬라이스**만큼 실행된 후, 대기 큐의 끝으로 이동합니다.  
- **선점(Preemptive)** 방식으로, 모든 프로세스가 공평하게 CPU 시간을 나눠 가집니다.  
- **장점:** 응답 시간이 빠르고 공평하게 분배됩니다.  
- **단점:** 타임 슬라이스가 너무 짧거나 길면 성능 저하가 발생할 수 있습니다.  

### 3. SJF (Shortest Job First)  
- 실행 시간이 가장 짧은 프로세스를 먼저 처리하는 **비선점(Non-preemptive)** 방식입니다.  
- **장점:** 전체 프로세스의 평균 대기 시간을 최소화합니다.  
- **단점:** 실행 시간이 긴 프로세스가 무기한 대기하는 **Starvation** 문제가 발생할 수 있습니다.  

### 4. SRTF (Shortest Remaining Time First)  
- 남은 실행 시간이 가장 짧은 프로세스를 먼저 처리하는 **선점(Preemptive)** 방식입니다.  
- 실행 중인 프로세스보다 짧은 작업이 도착하면, 현재 프로세스를 중단하고 새 작업을 실행합니다.  
- **장점:** SJF보다 더 효율적인 평균 대기 시간을 제공합니다.  
- **단점:** 작은 작업이 자주 도착하면 긴 작업이 계속 대기하게 됩니다.

</details>

<details>
  <summary>Eng</summary>

  ## Implementation and visualization of CPU scheduling algorithms using multithreading  


## 📋 Project Overview  
This project implements classic **CPU scheduling algorithms** in C language and visualizes their execution using **Java**. It is designed to help understand scheduling algorithms and observe performance differences in various scenarios.  

### Key Features  
- Implementation of scheduling algorithms using **multithreading**  
- Support for **FCFS**, **Round-Robin**, **SJF**, **SRTF** scheduling algorithms  
- **Real-time visualization** using a Java-based GUI  
- Visual representation of various processes in execution and waiting states  

## 🛠️ Implemented Scheduling Algorithms  

### 1. FCFS (First-Come, First-Served)  
- Processes are handled in the **order of arrival**, with the first process to arrive being processed first.  
- A **non-preemptive** approach, meaning no other process can run until the current process is complete.  
- **Pros:** Simple and intuitive to implement.  
- **Cons:** Can lead to long wait times, resulting in a **Convoy Effect**.  

### 2. Round-Robin (RR)  
- Processes are executed for a fixed **time slice** and then moved to the end of the queue.  
- A **preemptive** approach, ensuring all processes get equal CPU time.  
- **Pros:** Fast response time and fair time allocation.  
- **Cons:** If the time slice is too short or too long, performance can degrade.  

### 3. SJF (Shortest Job First)  
- The process with the shortest execution time is handled first, using a **non-preemptive** approach.  
- **Pros:** Minimizes the average waiting time for all processes.  
- **Cons:** Can lead to **Starvation** for longer processes.  

### 4. SRTF (Shortest Remaining Time First)  
- The process with the shortest remaining time is executed first, using a **preemptive** approach.  
- If a shorter job arrives, the current process is preempted to execute the new job.  
- **Pros:** Provides a more efficient average waiting time compared to SJF.  
- **Cons:** Long jobs may be continuously preempted by shorter jobs, leading to prolonged waiting.
</details>

## Mannual

<details>
  <summary>KOR</summary>

  ## 🚀 실행 방법

1. 리포지토리 클론:
   ```sh
   git clone <repository>
   ```
2. 디렉토리 이동:
   ```sh
   cd <repository-directory>
   ```
3. 프로그램 실행:
   ```sh
   make run
   ```

## 🎮 조작 방법

<img width="802" alt="스크린샷 2024-10-17 오전 12 44 54" src="https://github.com/user-attachments/assets/21bd0162-6fa9-4d9a-bf91-c9bde4548d9b">

1. 각 변수를 입력한 후 **Add** 버튼을 눌러 리스트에 추가합니다.
2. **Random** 버튼을 클릭하면 랜덤한 값이 추가됩니다.
3. **CONTEXT_SWITCH**는 문맥 교환 비용을 의미합니다.
4. **QUANTUM**은 time slice를 의미합니다.
5. **RQ_SIZE**는 라운드 로빈 정책의 큐 사이즈를 의미합니다.

### 선택 가능한 알고리즘

<img width="305" alt="스크린샷 2024-10-17 오전 12 45 13" src="https://github.com/user-attachments/assets/aff73e20-ff99-46c5-ba12-c87922014674">

- **lrrwp**: 제한된 우선순위 라운드 로빈 (Limited Round Robin with Priority)으로, 본 프로젝트에서 직접 개발한 알고리즘입니다.  
  *(상세 내용은 `lrrwp.c`를 참고하세요.)*

### 출력 내용

<img width="1202" alt="스크린샷 2024-10-17 오전 12 45 38" src="https://github.com/user-attachments/assets/b506c2c8-0d48-4ee0-9472-084e385f5ad5">

- 실행 로그, 그래프, 간단한 참고 변수들이 출력됩니다.
</details>

<details>
  <summary>ENG</summary>

  ## 🚀 How to Run

1. Clone the repository:
   ```sh
   git clone <repository>
   ```
2. Navigate to the directory:
   ```sh
   cd <repository-directory>
   ```
3. Run the program:
   ```sh
   make run
   ```

## 🎮 Controls

<img width="802" alt="스크린샷 2024-10-17 오전 12 44 54" src="https://github.com/user-attachments/assets/21bd0162-6fa9-4d9a-bf91-c9bde4548d9b">

1. Enter each variable and press the **Add** button to add it to the list.
2. Click the **Random** button to add random values.
3. **CONTEXT_SWITCH** represents the context switch cost.
4. **QUANTUM** represents the time slice.
5. **RQ_SIZE** represents the queue size for the Round-Robin policy.

### Available Algorithms

<img width="305" alt="스크린샷 2024-10-17 오전 12 45 13" src="https://github.com/user-attachments/assets/aff73e20-ff99-46c5-ba12-c87922014674">

- **lrrwp**: Limited Round Robin with Priority, a custom algorithm developed specifically for this project.  
  *(For more details, refer to `lrrwp.c`.)*

### Outputs

<img width="1202" alt="스크린샷 2024-10-17 오전 12 45 38" src="https://github.com/user-attachments/assets/b506c2c8-0d48-4ee0-9472-084e385f5ad5">

- Execution logs, graphs, and simple reference variables will be displayed.
</details>
