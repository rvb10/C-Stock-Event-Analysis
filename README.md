**Project Member :**

Rhugved Bhojane,
Juan Camilo Meneses,
Harsh Kulkarni,
Joaquin Garay,
Charlie Wu

**Project Deadlines:**

May 10, 2025-Submission Deadline

**Deliverables:**

Each team will submit PowerPoint slides and all project files, including source code and

executables, in a tar/zip format, to our course website three days before the presentation day.

- Presentation slides
- Source code + executables
- Output graphs (gnuplot)
- Weekly updates (by team leader)

Each team can resubmit their presentation once on the designated day if necessary.

Submission of project files in .tar/.zip format

**Evaluation Criteria:**

- Program Efficiency
- Complexity & Code Quality
- Presentation & Demo Performance
- Understanding of Code (individual quiz potential)

**Tools & Libraries:**

- C++ STL
- libcurl (for data retrieval)
- gnuplot (for graphing)

**Task 1: Earnings Research**

- Retrieve EPS data and earnings announcements from Zacks
- Sort Russell 3000 stocks into three categories:
    - Beat Estimate (High surprise%)
    - Meet Estimate (Medium surprise%)
    - Miss Estimate (Low surprise%)

**Task 2: Project Design**

- Create C++ Classes:
    - Stock (EPS data, prices)
    - Include necessary data structures (STL map, vector, matrix)
- Price Data Handling:
    - Use libcurl to fetch historical data from eodhistoricaldata.com
    - Benchmark: IWV (Russell 3000 ETF)
- Bootstrapping:
    - Select 30 stocks per group
    - Repeat the sampling 40 times
    - Retrieve 2n+1 days of price data (N between 30 and 60)
- Calculations:
    - Return: Rit = log(Pt / Pt-1)
    - Abnormal Return: ARit = Rit - Rmt
    - Average Abnormal Return (AAR)
    - Cumulative AAR (CAAR)
    - Standard Deviations for AAR and CAAR
- Implement Operator Overloading (e.g., for matrix operations)

**Task 3: Graphing with gnuplot**

- Plot CAAR curves for all three groups on a single chart

**Task 4: Menu Implementation**

- 5 Menu Options:
    1. Enter N for data retrieval (validate N: 30 ≤ N ≤ 60)
    2. Pull detailed information for one stock
    3. Display AAR/CAAR stats for one group
    4. Plot the CAAR comparison graph
    5. Exit

**Task 5: Module Division & Assignment**

- Allocate components among team members:
    - Data Retrieval
    - Earnings Processing
    - Bootstrapping + Calculations
    - Menu and Interface
    - Graphing Integration
    - Testing & Debugging

**Task 6: Integration & Testing**

- Combine all modules
- Verify the correctness of all outputs
- Conduct full system testing with varied N values and stock samples

**Task 7: Presentation Preparation**

- PowerPoint Contents:
    - Earnings research
    - UML diagrams
    - Class and data structure overview
    - Screenshots of outputs and graphs
    - Discussion of findings and conclusions