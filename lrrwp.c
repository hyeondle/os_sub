//Limited Round Robin with Priority

#include "scheduling.h"
#include "init.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

// 신규 정책 : 3개의 큐를 사용, 해당 큐만 라운드 로빈.
// 매 큐가 비었을 때 갱신은 우선순위가 높은 작업부터.
// 해당 우선순위는 러닝 큐 갱신 직전에 갱신한다.
// 기존 라운드 로빈에서 running_queue를 추가로 사용한다.
// running_queue는 러닝 큐가 비었을 때 갱신된다.
// running_queue가 비었을 때 ready_queue의 작업의 우선순위 갱신
// ready_queue의 작업의 우선순위가 갱신되면 제일 높은 우선순위의 작업을 running_queue로 이동
// 둘 다 비었을 때 종료상태 체크, 종료상태면 종료, 아니면 시간 증가
// 러닝 큐에 작업이 존재하면 flag = 1, 3개 다 있으면 3
//작성하기.
