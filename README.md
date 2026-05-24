#  WPC (Whey Protein Concentrate) Factory Simulation

OOP with C++ | GIST EECS

##  1. Project Overview

이 프로젝트는 C++ 기반의 객체지향 프로그래밍(OOP)을 활용하여 구현된 대화형 공장 자동화 시뮬레이션이다. 유청 단백질(WPC) 제조 공정을 주제로 선정하였으며, 사용자는 Dear ImGui로 구축된 직관적인 GUI를 통해 공정의 흐름과 기계의 상태를 실시간으로 제어하고 모니터링할 수 있다.

이 시뮬레이션은 객체지향의 4가지 핵심 원칙(추상화, 캡슐화, 상속, 다형성)을 중심으로 설계되었고, 복잡하고 상호작용이 많은 시스템에서 높은 유지보수성과 확장성을 가진다.

---

##  2. Core OOP Architecture (Design Justification)

### 1)  Polymorphism & Hierarchy
- 기저 클래스인 Machine을 추상 클래스로 정의하고, 공통 동작인 update()를 Template MEthod Pattern으로 구현했다. 메인 시뮬레이션 루프 (FactorySimulation::runTick()) 내에서는 각 기계의 구체적인 타입 (Ultrafiltration, Dryer, Packaging)을 식별하기 위한 어떤 if/else 분기나 dynamic_cast 없이 단일 기저 클래스 포인터 배열 (vector<Machine*>)을 순회하며 m->update()를 다형적으로 호출한다.

### 2) Open-Closed Principle)
- 새로운 공정기계를 추가할 때 오직 Machine의 클래스를 상속받아 새로운 서브클래스를 정의하고 구체적인 공정 로직 (process())와 고장 확률(breakdownChance())만 오버라이딩하면 된다. 메인 루프나 ImGui UI 파이프라인 코드는 추상화된 Machine 인터페이스에만 의존하기 때문에 백엔드 핵심 엔진과 UI 코드를 완전히 보존한 상태로 무한한 기능 확장이 가능하다.

### 3) Strict Encapsulation
- Machine 클래스 및 모든 시뮬레이션 관련 클래스의 멤버 변수(status, hp, processingTime, inputBuffer 등)는 private 또는 서브클래스 접근을 위한 protected로 제한되어 있습니다. 코드 리뷰어가 접근할 수 있는 public 데이터 멤버는 0개입니다. 상태 변화는 오직 캡슐화된 인터페이스 메서드(receiveItem(), repair(), forceBreak())를 통해서만 엄격하게 유도되므로 무결성이 보장됩니다.

### 4) UI & Backend Decoupling
- 백엔드 코어 클래스들(Machine, ItemBuffer 등) 내부에는 Dear ImGui 관련 헤더나 UI 의존성이 단 한 줄도 포함되어 있지 않습니다. 대신 MachineController 구조를 도입하여 UI 레이어가 백엔드 도메인 객체의 상태를 직접 오염시키지 못하도록 차단했습니다. 오직 최상위 진입점인 main.cpp만이 양방향을 인지하며 , UI 버튼 클릭 이벤트를 정제된 상태 가상화 메서드로 변환해 백엔드 엔진의 타이밍 규칙에 맞게 안전하게 전달합니다.


##  3. Factory Domain Scenario

시뮬레이션은 3단계의 핵심 파이프라인을 거쳐 액상 유청을 완제품 분말로 가공합니다. 각각의 기계는 고유한 처리 시간과 고장 확률을 가집니다.

1. **Ultrafiltration (한외여과기)**
* **역할:** 액상 유청에서 수분/유당을 거르고 단백질 농축 (공정 시간: 3 Ticks)
* **특징:** 필터 노후화로 인한 간헐적 고장 발생 가능 (고장 확률 10%)


2. **Spray Dryer (분무 건조기) - Bottleneck**
* **역할:** 농축액에 뜨거운 바람을 분사하여 가루로 변환 (공정 시간: 7 Ticks)
* **특징:** 처리 시간이 가장 길어 자연스러운 **병목(Bottleneck) 현상**을 유발합니다. 온도 센서 문제로 인해 고장 확률이 가장 높습니다 (20%).


3. **Packaging (포장기)**
* **역할:** 단백질 분말을 2kg 용기에 밀봉 (공정 시간: 2 Ticks)
* **특징:** 처리 속도가 가장 빠르며 고장이 발생하지 않습니다 (고장 확률 0%).

---

##  4. Simulation Scenarios

런타임 중 ImGui 드롭다운 메뉴를 통해 다음 시나리오들을 실시간으로 전환할 수 있습니다.

* 
**Normal Flow (정상 흐름):** * 파이프라인이 기본 속도로 가동됩니다. 각 기계의 처리 시간 차이(3 $\rightarrow$ 7 $\rightarrow$ 2)로 인해 2차 공정(건조기) 앞에 원료 대기열이 쌓이는 현실적인 병목 현상을 관찰할 수 있습니다.


* 
**Random Breakdowns (랜덤 고장 모드):** * 기계들의 고장 확률과 받는 데미지가 대폭 증가합니다. 극한의 환경에서 기계들이 연쇄적으로 가동을 멈추며, 이에 따라 작업물(WIP)이 어떻게 적체되는지(Overflow) 확인할 수 있습니다.



---

##  5. ImGui User Interface

요구사항에 맞추어 다음의 5가지 핵심 패널을 구현했습니다.

* 
**Simulation Control:** `Start`, `Pause`, `Reset` 버튼과 시뮬레이션 배속 슬라이더(1x-5x), 시나리오 선택기, 실시간 Tick 카운터를 제공합니다.


* 
**Factory Floor:** 공정 내 모든 기계의 현재 상태(WORKING, BROKEN, IDLE)를 색상 코드(Color-coded) 문자로 한눈에 보여줍니다.


* **Inspector:** 특정 기계를 클릭하면 체력 게이지(Health bar), 작업 진행률(Progress bar), 큐 대기열(Queue depth)을 상세히 표시합니다. 관리자는 `Force Break` 및 `Instant Repair` 버튼으로 직접 개입할 수 있습니다.


* **Event Log:** 기계의 상태 변화, 수리, 강제 고장 등의 모든 이벤트를 타임스탬프(Tick)와 함께 스크롤 가능한 창에 기록합니다. (`Clear Log` 기능 포함) .


* 
**Statistics:** 생산된 완제품(Finished goods) 수, 현재 공정 중인 재공품(WIP count), 누적 고장 횟수(Total breakdowns)를 실시간으로 집계합니다.



---

##  6. How to Build & Run

1. 본 리포지토리를 Clone 합니다.
2. CMake를 이용하여 빌드 환경을 구성합니다. (ImGui, SDL2, OpenGL3 연동 필요)
3. 빌드된 실행 파일을 통해 시뮬레이션을 시작합니다.

