================================================================================
          INTELLIGENT STUDENT MENTORSHIP ALLOCATION SYSTEM (DSA PROJECT)
================================================================================
Language   : C (C99 Standard)
Domain     : Data Structures and Algorithms (DSA) / Problem-Based Learning (PBL)
Libraries  : Standard C Libraries Only (<stdio.h>, <stdlib.h>, <string.h>, <ctype.h>)
Database   : None (In-Memory Arrays of Structures)

--------------------------------------------------------------------------------
1. PROJECT OVERVIEW
--------------------------------------------------------------------------------
The "Intelligent Student Mentorship Allocation System" is a menu-driven console
application designed to automate and optimize the process of pairing college
students with faculty mentors. 

In traditional college mentorship allocations, pairings are either done manually
or through arbitrary roll-number division, completely ignoring student technical
interests, career aspirations, and mentor field specializations. Furthermore,
mentors often get overloaded beyond their guidance capacity.

This project solves these issues using a deterministic Greedy Algorithm that:
1. Evaluates mutual compatibility across three dimensions:
   - Preferred Mentoring Domain (Weight: 50 points)
   - Technical Skill (Weight: 30 points)
   - Department / Branch (Weight: 20 points)
2. Assigns each student to the highest-scoring compatible mentor.
3. Strictly enforces mentor maximum student capacity constraints.
4. Breaks ties dynamically by selecting the mentor with the lighter workload
   (load balancing).
5. Seamlessly flags students as "Not Allocated" when capacity limits are hit or
   when no domain overlap exists.

The entire codebase is written in clean, standard, beginner-friendly C with zero
external dependencies, making it ideal for college DSA laboratory examinations,
viva voce evaluations, and mini-project demonstrations.

--------------------------------------------------------------------------------
2. DATA STRUCTURES IMPLEMENTATION
--------------------------------------------------------------------------------
The project leverages two core composite data structures (`struct`):

1. `Student` Structure:
   ----------------------------------------------------------------------
   Field Name              Type        Purpose
   ----------------------------------------------------------------------
   id                      int         Unique identification number
   name                    char[50]    Student's full name
   department              char[50]    Branch (e.g., CSE, IT, ECE, MECH)
   preferred_domain        char[50]    Area of interest (e.g., Machine Learning)
   technical_skill         char[50]    Proficient skill (e.g., Python, React)
   is_allocated            int         Flag (1 if assigned, 0 if unallocated)
   allocated_mentor_id     int         Assigned mentor ID (-1 if none)
   allocated_mentor_name   char[50]    Assigned mentor's name
   compatibility_score     int         Calculated compatibility score (0-100)
   ----------------------------------------------------------------------

2. `Mentor` Structure:
   ----------------------------------------------------------------------
   Field Name              Type        Purpose
   ----------------------------------------------------------------------
   id                      int         Unique identification number
   name                    char[50]    Mentor's full name
   department              char[50]    Academic department (e.g., CSE, IT, ECE)
   expertise               char[50]    Specialized domain of guidance
   max_capacity            int         Maximum number of students permitted
   currently_assigned      int         Active count of mentees allocated
   ----------------------------------------------------------------------

In-Memory Storage:
- `Student students[MAX_STUDENTS];` (Static array of structures, max 100)
- `Mentor mentors[MAX_MENTORS];`    (Static array of structures, max 50)
- Count tracking variables: `student_count` and `mentor_count`.

--------------------------------------------------------------------------------
3. MAIN FEATURES & MENU OPTIONS
--------------------------------------------------------------------------------
The system presents a clean, numbered menu:

1.  Add Student                  : Register a student with validation and unique ID.
2.  Display All Students         : Tabular overview of all registered students.
3.  Add Mentor                   : Register a faculty mentor with max capacity.
4.  Display All Mentors          : Tabular overview of all faculty mentors.
5.  Search Student by ID         : Linear search showing full profile & assignment.
6.  Search Mentor by ID          : Linear search showing capacity & assigned mentees.
7.  Allocate Mentors to Students : Greedy allocation engine with capacity limits.
8.  Display Allocation Results   : Formatted results table (ID, Name, Mentor, Score).
9.  Display Mentor Workload      : Workload utilization, remaining slots & status.
10. Sort Students by ID          : Bubble sort with early-termination optimization.
11. Reset Allocations            : Clears allocations and resets mentor workloads.
12. Exit                         : Graceful program termination.

* Special Feature: Enter '0' at the main menu (or answer '1' at startup) to load
  a pre-configured demonstration dataset with 5 students and 3 mentors.

--------------------------------------------------------------------------------
4. INTELLIGENT ALLOCATION ALGORITHM (GREEDY STRATEGY)
--------------------------------------------------------------------------------
The allocation engine uses a Greedy Heuristic to maximize immediate compatibility
for each student while honoring mentor constraints:

Compatibility Score Matrix (Maximum = 100 points):
----------------------------------------------------------------------
Criteria                            Points Awarded   Rationale
----------------------------------------------------------------------
Preferred Domain == Mentor Expertise      50         Core alignment of research
Technical Skill == Mentor Expertise       30         Student prerequisite skill
Department == Mentor Department           20         Administrative convenience
----------------------------------------------------------------------

Algorithm Rules:
1. Iterate through each student i = 0 to (student_count - 1).
2. For student i, iterate through all mentors j = 0 to (mentor_count - 1).
3. Candidate check:
   - Mentor j is eligible ONLY IF:
     (mentors[j].currently_assigned < mentors[j].max_capacity)
4. If eligible, compute compatibility score:
   Score = DomainMatch(50) + SkillMatch(30) + DeptMatch(20)
5. Selection Criteria:
   - Choose the mentor with the highest score > 0.
   - Tie-breaking Rule: If two mentors have identical compatibility scores,
     select the mentor with FEWER currently assigned students (Load Balancing).
6. State Update:
   - If a mentor is found:
       student.is_allocated = 1
       student.allocated_mentor_id = mentor.id
       student.allocated_mentor_name = mentor.name
       student.compatibility_score = max_score
       mentor.currently_assigned++ (Updated immediately!)
   - If no eligible mentor exists or highest score is 0:
       student.is_allocated = 0
       student.allocated_mentor_name = "Not Available"
       student.compatibility_score = 0
7. Duplicate Allocation Prevention:
   - System prevents executing allocation multiple times sequentially.
   - The user must explicitly choose Option 11 (Reset Allocations) to re-run.

--------------------------------------------------------------------------------
5. PSEUDOCODE
--------------------------------------------------------------------------------

Greedy Allocation Algorithm:
----------------------------
FUNCTION allocateMentors():
    IF student_count == 0 OR mentor_count == 0 THEN
        PRINT "Insufficient data to run allocation"
        RETURN
    END IF

    IF is_allocation_done == TRUE THEN
        PRINT "Allocations already completed. Reset first."
        RETURN
    END IF

    FOR i FROM 0 TO (student_count - 1) DO:
        best_mentor_index = -1
        max_score = 0

        FOR j FROM 0 TO (mentor_count - 1) DO:
            IF mentors[j].currently_assigned < mentors[j].max_capacity THEN
                score = calculateCompatibilityScore(students[i], mentors[j])
                
                IF score > 0 THEN
                    IF score > max_score THEN
                        max_score = score
                        best_mentor_index = j
                    ELSE IF score == max_score AND best_mentor_index != -1 THEN
                        // Load balancing tie-breaker
                        IF mentors[j].currently_assigned < mentors[best_mentor_index].currently_assigned THEN
                            best_mentor_index = j
                        END IF
                    END IF
                END IF
            END IF
        END FOR

        IF best_mentor_index != -1 THEN
            students[i].is_allocated = 1
            students[i].allocated_mentor_id = mentors[best_mentor_index].id
            students[i].allocated_mentor_name = mentors[best_mentor_index].name
            students[i].compatibility_score = max_score
            mentors[best_mentor_index].currently_assigned = mentors[best_mentor_index].currently_assigned + 1
        ELSE
            students[i].is_allocated = 0
            students[i].allocated_mentor_id = -1
            students[i].allocated_mentor_name = "Not Available"
            students[i].compatibility_score = 0
        END IF
    END FOR

    is_allocation_done = TRUE
    PRINT "Allocation completed successfully"
END FUNCTION

Optimized Bubble Sort Algorithm (Sort by Student ID):
-----------------------------------------------------
FUNCTION sortStudentsById():
    IF student_count <= 1 THEN
        RETURN
    END IF

    FOR i FROM 0 TO (student_count - 2) DO:
        swapped = FALSE
        FOR j FROM 0 TO (student_count - 2 - i) DO:
            IF students[j].id > students[j + 1].id THEN
                SWAP(students[j], students[j + 1])
                swapped = TRUE
            END IF
        END FOR
        // If no two elements were swapped, array is already sorted
        IF swapped == FALSE THEN
            BREAK
        END IF
    END FOR
END FUNCTION

Linear Search Algorithm:
------------------------
FUNCTION searchStudentById(search_id):
    found_index = -1
    FOR i FROM 0 TO (student_count - 1) DO:
        IF students[i].id == search_id THEN
            found_index = i
            BREAK
        END IF
    END FOR
    RETURN found_index
END FUNCTION

--------------------------------------------------------------------------------
6. DATA STRUCTURES & ALGORITHMS (DSA) CONCEPTS & COMPLEXITY ANALYSIS
--------------------------------------------------------------------------------

1. Structures (Record Management):
   - Encapsulates heterogeneous attributes (integer ID, strings for names and
     domains, allocation states) into unified logical entities (`Student`, `Mentor`).

2. Arrays of Structures (In-Memory Database):
   - Contiguous memory allocation providing direct O(1) indexed access:
     `students[i]`, `mentors[j]`.

3. Linear Search:
   - Used in `searchStudentById()` and `searchMentorById()`.
   - Iterates sequentially from index 0 to N-1.
   - Best Case Time Complexity  : O(1) [Element found at index 0]
   - Worst Case Time Complexity : O(N) [Element at last index or not present]
   - Average Case Complexity    : O(N)
   - Space Complexity           : O(1)

4. Bubble Sort:
   - Used in `sortStudentsById()` to arrange students in ascending order of ID.
   - Compares adjacent pairs and bubbles the largest unsorted ID to the end.
   - Best Case Time Complexity  : O(N) [When array is already sorted, flag exits]
   - Worst Case Time Complexity : O(N^2) [Array sorted in reverse order]
   - Average Case Complexity    : O(N^2)
   - Auxiliary Space Complexity : O(1) [In-place swapping, zero extra memory]

5. Greedy Compatibility Allocation:
   - For every student (N), scans all mentors (M) to select the locally optimal
     mentor available at that instant.
   - Time Complexity            : O(N * M)
     Where N = number of students, M = number of mentors.
     For college-scale batches (e.g., N=100, M=50), operations = 5,000 checks,
     executing in under 1 millisecond.
   - Space Complexity           : O(1) auxiliary [Updates records in-place].

--------------------------------------------------------------------------------
7. SAMPLE DEMONSTRATION DATASET & ALLOCATION ANALYSIS
--------------------------------------------------------------------------------
The pre-loaded demonstration data contains 3 Mentors and 5 Students specifically
engineered to prove all real-world allocation dynamics:

Mentors Registered:
1. ID: 201 | Amit   | Dept: CSE | Expertise: Machine Learning | Capacity: 2
2. ID: 202 | Neha   | Dept: IT  | Expertise: Web Development | Capacity: 1 (Tight)
3. ID: 203 | Rajesh | Dept: ECE | Expertise: Embedded Systems| Capacity: 1

Students Registered & Pairings:
1. Student 101: Rahul
   - Dept: ECE | Domain: Machine Learning | Skill: Machine Learning
   - Match with Amit: Domain(50) + Skill(30) + Dept(0) = 80 Points
   - Result: ALLOCATED to Amit (Amit workload: 1/2)

2. Student 102: Priya
   - Dept: IT | Domain: Web Development | Skill: Web Development
   - Match with Neha: Domain(50) + Skill(30) + Dept(20) = 100 Points (Full Match)
   - Result: ALLOCATED to Neha (Neha workload: 1/1 -> FULL)

3. Student 103: Aman
   - Dept: Mechanical | Domain: Robotics | Skill: CAD Design
   - Match with all mentors = 0 Points (No matching technical domain)
   - Result: NOT ALLOCATED (Assigned Mentor: "Not Available", Score: 0)

4. Student 104: Sneha
   - Dept: IT | Domain: Web Development | Skill: HTML/CSS
   - Match with Neha: Domain(50) + Dept(20) = 70 Points
   - BUT Mentor Neha capacity (1) is ALREADY SATURATED by Priya!
   - Result: NOT ALLOCATED (Demonstrates Mentor Capacity Saturation!)

5. Student 105: Vikram
   - Dept: CSE | Domain: Machine Learning | Skill: Python
   - Match with Amit: Domain(50) + Dept(20) = 70 Points
   - Amit still has 1 remaining slot (1/2 -> 2/2)
   - Result: ALLOCATED to Amit (Amit reaches maximum capacity!)

--------------------------------------------------------------------------------
8. COMPILATION AND EXECUTION INSTRUCTIONS
--------------------------------------------------------------------------------

Requirement: Any standard C compiler (GCC, Clang, or MSVC).

Method 1: GCC on Linux / macOS / Windows (MinGW / Git Bash / WSL)
----------------------------------------------------------------
Open terminal in the project directory:

  # 1. Compile the source code
  gcc main.c -o mentorship

  # Optional: Compile with strict warnings
  gcc -Wall -Wextra main.c -o mentorship

  # 2. Run the application
  # Linux / macOS / Git Bash:
  ./mentorship

  # Windows Command Prompt / PowerShell:
  .\mentorship.exe

Method 2: Clang
---------------
  clang main.c -o mentorship
  ./mentorship

Method 3: Microsoft Visual C++ (Developer Command Prompt)
---------------------------------------------------------
  cl main.c /Fe:mentorship.exe
  .\mentorship.exe

Method 4: Code::Blocks / Dev-C++ / VS Code
------------------------------------------
1. Open the folder in Code::Blocks / Dev-C++.
2. Open `main.c`.
3. Click "Build and Run" (F9 in Code::Blocks / F11 in Dev-C++).

--------------------------------------------------------------------------------
9. SAMPLE PROGRAM RUN OUTPUT
--------------------------------------------------------------------------------

Startup Screen:
========================================================================
       INTELLIGENT STUDENT MENTORSHIP ALLOCATION SYSTEM (C / DSA)       
========================================================================
Welcome! This system optimizes mentorship pairings using a Greedy
Compatibility Scoring Algorithm with capacity limits and load balancing.
------------------------------------------------------------------------
Would you like to load default sample demonstration data now?
(Contains 5 realistic students & 3 mentors to demonstrate allocations,
 capacity saturation, and unallocated edge cases)
Enter 1 for YES, 0 for NO: 1

[+] Sample demonstration dataset loaded successfully!
    - 3 Mentors (IDs: 201 [Amit], 202 [Neha], 203 [Rajesh])
    - 5 Students (IDs: 101 [Rahul], 102 [Priya], 103 [Aman], 104 [Sneha], 105 [Vikram])
    Tip: Use Option 2 and Option 4 to view loaded students and mentors,
    then run Option 7 (Allocate) to observe the intelligent matching.

Option 7 (Allocation Run):
========================================================================
            RUNNING INTELLIGENT GREEDY ALLOCATION ALGORITHM             
========================================================================
Scoring Rules:
  - Preferred Mentoring Domain Match : 50 points
  - Technical Skill Match            : 30 points
  - Department / Branch Match        : 20 points
Strategy:
  Greedily selects the mentor with highest compatibility score.
  Tie-breaker: Chooses mentor with fewer assigned students (load balance).
  Capacity Rule: Cannot exceed mentor's maximum student capacity.
------------------------------------------------------------------------
[+] Allocation process completed successfully!
    - Total Students Processed : 5
    - Successfully Allocated   : 3
    - Unallocated (Full/No Fit): 2
------------------------------------------------------------------------

Option 8 (Display Allocation Results):
========================================================================
                 STUDENT MENTORSHIP ALLOCATION RESULTS                  
========================================================================
Student ID   Student Name     Assigned Mentor    Score    Status         
------------------------------------------------------------------------
101          Rahul            Amit               80       Allocated      
102          Priya            Neha               100      Allocated      
103          Aman             Not Available      0        Not Allocated  
104          Sneha            Not Available      0        Not Allocated  
105          Vikram           Amit               70       Allocated      
========================================================================

Option 9 (Display Mentor Workload):
=========================================================================================
                                   MENTOR WORKLOAD REPORT                                
=========================================================================================
Mentor ID  Mentor Name        Department   Capacity   Assigned   Available    Utilization       
-----------------------------------------------------------------------------------------
201        Amit               CSE          2          2          0            100% [FULL]       
202        Neha               IT           1          1          0            100% [FULL]       
203        Rajesh             ECE          1          0          1              0% (1 slots left)
=========================================================================================
Total Mentor Capacity: 4 | Total Assigned: 3 | Total Free Slots: 1

Option 10 (Sort Students by ID - Bubble Sort):
========================================================================
                  SORTING STUDENTS BY ID (BUBBLE SORT)                  
========================================================================
Algorithm       : Bubble Sort
Operation       : Repeatedly steps through list, compares adjacent items
                  and swaps them if in wrong order.
Time Complexity : Best Case O(N), Average/Worst Case O(N^2)
Space Complexity: O(1) Auxiliary In-place sorting
------------------------------------------------------------------------
[+] Sorting completed successfully in 1 pass(es) with 0 total swap(s).

========================================================================================
                                  REGISTERED STUDENTS LIST                              
========================================================================================
Student ID Student Name         Department   Preferred Domain         Technical Skill     
----------------------------------------------------------------------------------------
101        Rahul                ECE          Machine Learning         Machine Learning    
102        Priya                IT           Web Development          Web Development     
103        Aman                 Mechanical   Robotics                 CAD Design          
104        Sneha                IT           Web Development          HTML/CSS            
105        Vikram               CSE          Machine Learning         Python              
========================================================================================

--------------------------------------------------------------------------------
10. COLLEGE VIVA VOCE & EVALUATION QUESTIONS & ANSWERS
--------------------------------------------------------------------------------
Q1: Why is this allocation called a "Greedy Algorithm"?
A1: Because for every student, it immediately selects the available mentor with
    the highest local compatibility score without backtracking or waiting to
    inspect future students.

Q2: What happens if two mentors have the exact same compatibility score?
A2: The tie-breaker rule activates: the system chooses the mentor who currently
    has fewer assigned mentees, thereby balancing faculty workload.

Q3: Why did Student 104 (Sneha) remain unallocated even though she matched Neha?
A3: Mentor Neha had a maximum capacity of 1 student, which was already claimed
    by Student 102 (Priya) who had a higher compatibility score (100 vs 70).
    Since no other mentor matched Sneha's domain, she was marked "Not Allocated".

Q4: What is the time complexity of the allocation algorithm?
A4: O(N * M), where N is the number of students and M is the number of mentors.
    Each of the N students evaluates up to M mentors.

Q5: Why did you choose Bubble Sort over Quick Sort or Merge Sort?
A5: Bubble Sort is simple, requires zero auxiliary memory (in-place O(1) space),
    and is exceptionally clear to explain during academic examinations. The
    implemented version includes a `swapped` flag optimization that runs in
    O(N) best-case time if the list is already sorted.

Q6: How does the system prevent buffer overflows and crash loops on invalid input?
A6: Instead of unsafe functions like `gets()` or raw `scanf("%s")`, we use `fgets()`
    with explicit size bounds, and a dedicated `readInteger()` function that parses
    input with `sscanf()` and discards stray characters from `stdin`.

Q7: What data structures did you use and why?
A7: We used C structures (`struct Student` and `struct Mentor`) to group diverse
    fields together, stored in contiguous arrays of structures for O(1) random
    indexed access and memory efficiency without needing external database drivers.

Q8: How does the system prevent duplicate allocations?
A8: An internal tracking flag `is_allocation_done` prevents running Option 7
    repeatedly. If students or mentors are updated, Option 11 (Reset Allocations)
    must be run to reset all mentor workloads and clear previous pairings.

================================================================================
                         END OF DOCUMENTATION FILE
================================================================================
