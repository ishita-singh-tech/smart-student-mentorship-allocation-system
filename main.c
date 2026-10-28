/*
 * ============================================================================
 * Project Title : Intelligent Student Mentorship Allocation System
 * Language      : C (C99 Standard)
 * Description   : A menu-driven console application that allocates students to
 *                 the most suitable mentors based on preferred domain, technical
 *                 skills, and department, utilizing a greedy compatibility scoring
 *                 algorithm with mentor capacity constraints and load balancing.
 * DSA Concepts  : Structures, Arrays of Structures, Linear Search,
 *                 Bubble Sort, Greedy Algorithm, Complexity Analysis.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define MAX_MENTORS  50
#define STR_LEN      50

/* ----------------------------------------------------------------------------
 * DATA STRUCTURES
 * ---------------------------------------------------------------------------- */

/* Structure representing a Student */
typedef struct {
    int id;                          /* Unique Student ID */
    char name[STR_LEN];              /* Student Full Name */
    char department[STR_LEN];        /* Department / Branch (e.g., CSE, IT, ECE) */
    char preferred_domain[STR_LEN];  /* Desired Mentoring Domain (e.g., Web Dev, ML) */
    char technical_skill[STR_LEN];   /* Primary Technical Skill */
    
    /* Allocation Fields */
    int is_allocated;                /* 1 = Allocated, 0 = Not Allocated */
    int allocated_mentor_id;         /* ID of Assigned Mentor (-1 if not allocated) */
    char allocated_mentor_name[STR_LEN]; /* Name of Assigned Mentor */
    int compatibility_score;         /* Calculated Compatibility Score (0-100) */
} Student;

/* Structure representing a Mentor */
typedef struct {
    int id;                          /* Unique Mentor ID */
    char name[STR_LEN];              /* Mentor Full Name */
    char department[STR_LEN];        /* Department / Branch */
    char expertise[STR_LEN];         /* Domain Expertise */
    int max_capacity;                /* Maximum students this mentor can guide */
    int currently_assigned;          /* Current count of assigned students */
} Mentor;

/* ----------------------------------------------------------------------------
 * GLOBAL IN-MEMORY REPOSITORIES
 * ---------------------------------------------------------------------------- */
static Student students[MAX_STUDENTS];
static int student_count = 0;

static Mentor mentors[MAX_MENTORS];
static int mentor_count = 0;

static int is_allocation_done = 0;   /* Tracks if allocation was executed */

/* ----------------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------------- */
void addStudent(void);
void displayAllStudents(void);
void addMentor(void);
void displayAllMentors(void);
void searchStudentById(void);
void searchMentorById(void);
void allocateMentors(void);
void displayAllocationResults(void);
void displayMentorWorkload(void);
void sortStudentsById(void);
void resetAllocations(void);
void loadSampleData(void);
void printMenu(void);

/* Helper / Utility Functions */
int findStudentIndexById(int id);
int findMentorIndexById(int id);
int calculateCompatibilityScore(const Student *s, const Mentor *m);
int stringEqualsIgnoreCase(const char *s1, const char *s2);
int stringContainsIgnoreCase(const char *haystack, const char *needle);
int stringMatch(const char *s1, const char *s2);
void readString(const char *prompt, char *buffer, int max_len);
int readInteger(const char *prompt);

/* ----------------------------------------------------------------------------
 * MAIN FUNCTION
 * ---------------------------------------------------------------------------- */
int main(void) {
    int choice;

    printf("========================================================================\n");
    printf("       INTELLIGENT STUDENT MENTORSHIP ALLOCATION SYSTEM (C / DSA)       \n");
    printf("========================================================================\n");
    printf("Welcome! This system optimizes mentorship pairings using a Greedy\n");
    printf("Compatibility Scoring Algorithm with capacity limits and load balancing.\n");
    printf("------------------------------------------------------------------------\n");
    printf("Would you like to load default sample demonstration data now?\n");
    printf("(Contains 5 realistic students & 3 mentors to demonstrate allocations,\n");
    printf(" capacity saturation, and unallocated edge cases)\n");
    printf("Enter 1 for YES, 0 for NO: ");

    int initial_load = readInteger("");
    if (initial_load == 1) {
        loadSampleData();
    } else {
        printf("\n[*] Starting with empty student and mentor records.\n");
    }

    while (1) {
        printMenu();
        choice = readInteger("Enter your choice (1-12): ");

        switch (choice) {
            case 1:
                addStudent();
                break;
            case 2:
                displayAllStudents();
                break;
            case 3:
                addMentor();
                break;
            case 4:
                displayAllMentors();
                break;
            case 5:
                searchStudentById();
                break;
            case 6:
                searchMentorById();
                break;
            case 7:
                allocateMentors();
                break;
            case 8:
                displayAllocationResults();
                break;
            case 9:
                displayMentorWorkload();
                break;
            case 10:
                sortStudentsById();
                break;
            case 11:
                resetAllocations();
                break;
            case 12:
                printf("\n========================================================================\n");
                printf("  Exiting Intelligent Student Mentorship Allocation System. Goodbye!    \n");
                printf("========================================================================\n");
                return 0;
            case 0:
                /* Shortcut to reload sample demonstration data */
                loadSampleData();
                break;
            default:
                printf("\n[!] Invalid menu choice! Please select an option between 1 and 12.\n");
                break;
        }
    }

    return 0;
}

/* ----------------------------------------------------------------------------
 * MENU DISPLAY
 * ---------------------------------------------------------------------------- */
void printMenu(void) {
    printf("\n========================================================================\n");
    printf("                              MAIN MENU                                 \n");
    printf("========================================================================\n");
    printf(" 1.  Add Student\n");
    printf(" 2.  Display All Students\n");
    printf(" 3.  Add Mentor\n");
    printf(" 4.  Display All Mentors\n");
    printf(" 5.  Search Student by ID (Linear Search)\n");
    printf(" 6.  Search Mentor by ID (Linear Search)\n");
    printf(" 7.  Allocate Mentors to Students (Greedy Algorithm)\n");
    printf(" 8.  Display Allocation Results\n");
    printf(" 9.  Display Mentor Workload\n");
    printf(" 10. Sort Students by ID (Bubble Sort)\n");
    printf(" 11. Reset Allocations\n");
    printf(" 12. Exit\n");
    printf("------------------------------------------------------------------------\n");
    printf(" (Tip: Enter 0 anytime to load/reload sample demo dataset)\n");
    printf("========================================================================\n");
}

/* ----------------------------------------------------------------------------
 * 1. ADD STUDENT
 * ---------------------------------------------------------------------------- */
void addStudent(void) {
    if (student_count >= MAX_STUDENTS) {
        printf("\n[!] Error: Maximum student capacity (%d) reached. Cannot add more.\n", MAX_STUDENTS);
        return;
    }

    printf("\n-------------------- ADD NEW STUDENT --------------------\n");
    int id = readInteger("Enter Student ID (positive integer): ");

    if (id <= 0) {
        printf("[!] Error: Student ID must be a positive number.\n");
        return;
    }

    if (findStudentIndexById(id) != -1) {
        printf("[!] Error: Student with ID %d already exists! Duplicate IDs are rejected.\n", id);
        return;
    }

    Student s;
    s.id = id;
    readString("Enter Student Name: ", s.name, STR_LEN);
    readString("Enter Department (e.g., CSE, IT, ECE): ", s.department, STR_LEN);
    readString("Enter Preferred Mentoring Domain (e.g., Machine Learning, Web Development): ", s.preferred_domain, STR_LEN);
    readString("Enter Technical Skill (e.g., Python, React, Embedded C): ", s.technical_skill, STR_LEN);

    /* Initialize allocation attributes */
    s.is_allocated = 0;
    s.allocated_mentor_id = -1;
    strcpy(s.allocated_mentor_name, "Not Available");
    s.compatibility_score = 0;

    students[student_count] = s;
    student_count++;

    printf("\n[+] Success: Student '%s' (ID: %d) added successfully!\n", s.name, s.id);

    if (is_allocation_done) {
        printf("    Note: Allocations were already performed earlier. Run Option 11 (Reset)\n");
        printf("    and Option 7 (Allocate) to include this new student in mentorship pairings.\n");
    }
}

/* ----------------------------------------------------------------------------
 * 2. DISPLAY ALL STUDENTS
 * ---------------------------------------------------------------------------- */
void displayAllStudents(void) {
    if (student_count == 0) {
        printf("\n[!] No students registered in the system yet.\n");
        return;
    }

    printf("\n========================================================================================\n");
    printf("                                  REGISTERED STUDENTS LIST                              \n");
    printf("========================================================================================\n");
    printf("%-10s %-20s %-12s %-24s %-20s\n",
           "Student ID", "Student Name", "Department", "Preferred Domain", "Technical Skill");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < student_count; i++) {
        printf("%-10d %-20s %-12s %-24s %-20s\n",
               students[i].id,
               students[i].name,
               students[i].department,
               students[i].preferred_domain,
               students[i].technical_skill);
    }

    printf("========================================================================================\n");
    printf("Total Students Registered: %d / %d\n", student_count, MAX_STUDENTS);
}

/* ----------------------------------------------------------------------------
 * 3. ADD MENTOR
 * ---------------------------------------------------------------------------- */
void addMentor(void) {
    if (mentor_count >= MAX_MENTORS) {
        printf("\n[!] Error: Maximum mentor capacity (%d) reached. Cannot add more.\n", MAX_MENTORS);
        return;
    }

    printf("\n-------------------- ADD NEW MENTOR --------------------\n");
    int id = readInteger("Enter Mentor ID (positive integer): ");

    if (id <= 0) {
        printf("[!] Error: Mentor ID must be a positive number.\n");
        return;
    }

    if (findMentorIndexById(id) != -1) {
        printf("[!] Error: Mentor with ID %d already exists! Duplicate IDs are rejected.\n", id);
        return;
    }

    Mentor m;
    m.id = id;
    readString("Enter Mentor Name: ", m.name, STR_LEN);
    readString("Enter Department (e.g., CSE, IT, ECE): ", m.department, STR_LEN);
    readString("Enter Domain Expertise (e.g., Machine Learning, Web Development): ", m.expertise, STR_LEN);

    m.max_capacity = readInteger("Enter Maximum Student Capacity (e.g., 2 to 5): ");
    while (m.max_capacity <= 0) {
        printf("[!] Maximum capacity must be at least 1.\n");
        m.max_capacity = readInteger("Enter Maximum Student Capacity: ");
    }

    m.currently_assigned = 0;

    mentors[mentor_count] = m;
    mentor_count++;

    printf("\n[+] Success: Mentor '%s' (ID: %d, Capacity: %d) added successfully!\n",
           m.name, m.id, m.max_capacity);

    if (is_allocation_done) {
        printf("    Note: Allocations were already performed earlier. Run Option 11 (Reset)\n");
        printf("    and Option 7 (Allocate) to include this new mentor in pairings.\n");
    }
}

/* ----------------------------------------------------------------------------
 * 4. DISPLAY ALL MENTORS
 * ---------------------------------------------------------------------------- */
void displayAllMentors(void) {
    if (mentor_count == 0) {
        printf("\n[!] No mentors registered in the system yet.\n");
        return;
    }

    printf("\n=========================================================================================\n");
    printf("                                   REGISTERED MENTORS LIST                               \n");
    printf("=========================================================================================\n");
    printf("%-10s %-20s %-12s %-24s %-10s %-10s\n",
           "Mentor ID", "Mentor Name", "Department", "Expertise", "Max Cap", "Assigned");
    printf("-----------------------------------------------------------------------------------------\n");

    int total_capacity = 0;
    int total_assigned = 0;

    for (int i = 0; i < mentor_count; i++) {
        printf("%-10d %-20s %-12s %-24s %-10d %-10d\n",
               mentors[i].id,
               mentors[i].name,
               mentors[i].department,
               mentors[i].expertise,
               mentors[i].max_capacity,
               mentors[i].currently_assigned);
        total_capacity += mentors[i].max_capacity;
        total_assigned += mentors[i].currently_assigned;
    }

    printf("=========================================================================================\n");
    printf("Total Mentors: %d | Total Capacity: %d slots | Currently Assigned: %d slots\n",
           mentor_count, total_capacity, total_assigned);
}

/* ----------------------------------------------------------------------------
 * 5. SEARCH STUDENT BY ID (Linear Search)
 * ---------------------------------------------------------------------------- */
void searchStudentById(void) {
    if (student_count == 0) {
        printf("\n[!] No students registered yet to search.\n");
        return;
    }

    int search_id = readInteger("\nEnter Student ID to search: ");
    int comparisons = 0;
    int found_index = -1;

    /* Linear Search Algorithm */
    for (int i = 0; i < student_count; i++) {
        comparisons++;
        if (students[i].id == search_id) {
            found_index = i;
            break;
        }
    }

    printf("\n------------------- SEARCH RESULT -------------------\n");
    printf("Algorithm Used      : Linear Search\n");
    printf("Time Complexity     : O(N) [Comparisons performed: %d / %d]\n", comparisons, student_count);
    printf("-----------------------------------------------------\n");

    if (found_index != -1) {
        Student *s = &students[found_index];
        printf("Search Status       : Record Found\n");
        printf("Student ID          : %d\n", s->id);
        printf("Student Name        : %s\n", s->name);
        printf("Department          : %s\n", s->department);
        printf("Preferred Domain    : %s\n", s->preferred_domain);
        printf("Technical Skill     : %s\n", s->technical_skill);
        printf("Allocation Status   : %s\n", s->is_allocated ? "Allocated" : "Not Allocated");
        printf("Assigned Mentor     : %s\n", s->allocated_mentor_name);
        if (s->is_allocated) {
            printf("Mentor ID           : %d\n", s->allocated_mentor_id);
            printf("Compatibility Score : %d / 100\n", s->compatibility_score);
        }
    } else {
        printf("Search Status       : Not Found\n");
        printf("Student with ID %d does not exist in the system.\n", search_id);
    }
    printf("-----------------------------------------------------\n");
}

/* ----------------------------------------------------------------------------
 * 6. SEARCH MENTOR BY ID (Linear Search)
 * ---------------------------------------------------------------------------- */
void searchMentorById(void) {
    if (mentor_count == 0) {
        printf("\n[!] No mentors registered yet to search.\n");
        return;
    }

    int search_id = readInteger("\nEnter Mentor ID to search: ");
    int comparisons = 0;
    int found_index = -1;

    /* Linear Search Algorithm */
    for (int i = 0; i < mentor_count; i++) {
        comparisons++;
        if (mentors[i].id == search_id) {
            found_index = i;
            break;
        }
    }

    printf("\n------------------- SEARCH RESULT -------------------\n");
    printf("Algorithm Used      : Linear Search\n");
    printf("Time Complexity     : O(M) [Comparisons performed: %d / %d]\n", comparisons, mentor_count);
    printf("-----------------------------------------------------\n");

    if (found_index != -1) {
        Mentor *m = &mentors[found_index];
        int avail = m->max_capacity - m->currently_assigned;
        printf("Search Status       : Record Found\n");
        printf("Mentor ID           : %d\n", m->id);
        printf("Mentor Name         : %s\n", m->name);
        printf("Department          : %s\n", m->department);
        printf("Domain Expertise    : %s\n", m->expertise);
        printf("Maximum Capacity    : %d\n", m->max_capacity);
        printf("Currently Assigned  : %d\n", m->currently_assigned);
        printf("Available Slots     : %d\n", avail);

        printf("\nMentees Currently Assigned to %s:\n", m->name);
        int mentee_count = 0;
        for (int i = 0; i < student_count; i++) {
            if (students[i].is_allocated && students[i].allocated_mentor_id == m->id) {
                mentee_count++;
                printf("  %d. [ID: %d] %-18s (Dept: %s | Match Score: %d)\n",
                       mentee_count, students[i].id, students[i].name,
                       students[i].department, students[i].compatibility_score);
            }
        }
        if (mentee_count == 0) {
            printf("  (No students assigned to this mentor yet)\n");
        }
    } else {
        printf("Search Status       : Not Found\n");
        printf("Mentor with ID %d does not exist in the system.\n", search_id);
    }
    printf("-----------------------------------------------------\n");
}

/* ----------------------------------------------------------------------------
 * 7. ALLOCATE MENTORS TO STUDENTS (Greedy Algorithm)
 * ---------------------------------------------------------------------------- */
void allocateMentors(void) {
    if (student_count == 0) {
        printf("\n[!] Cannot run allocation: No students registered in the system.\n");
        return;
    }

    if (mentor_count == 0) {
        printf("\n[!] Cannot run allocation: No mentors registered in the system.\n");
        return;
    }

    /* Prevent duplicate execution */
    if (is_allocation_done) {
        printf("\n[!] Allocations have already been executed!\n");
        printf("    To re-run the allocation algorithm, please select Option 11 (Reset Allocations)\n");
        printf("    first to clear existing allocations and reset mentor workloads.\n");
        return;
    }

    printf("\n========================================================================\n");
    printf("            RUNNING INTELLIGENT GREEDY ALLOCATION ALGORITHM             \n");
    printf("========================================================================\n");
    printf("Scoring Rules:\n");
    printf("  - Preferred Mentoring Domain Match : 50 points\n");
    printf("  - Technical Skill Match            : 30 points\n");
    printf("  - Department / Branch Match        : 20 points\n");
    printf("Strategy:\n");
    printf("  Greedily selects the mentor with highest compatibility score.\n");
    printf("  Tie-breaker: Chooses mentor with fewer assigned students (load balance).\n");
    printf("  Capacity Rule: Cannot exceed mentor's maximum student capacity.\n");
    printf("------------------------------------------------------------------------\n");

    int allocated_count = 0;
    int unallocated_count = 0;

    /* For each student, find the best available mentor */
    for (int i = 0; i < student_count; i++) {
        int best_mentor_index = -1;
        int max_score = 0;

        for (int j = 0; j < mentor_count; j++) {
            /* Only consider mentors who have free capacity */
            if (mentors[j].currently_assigned < mentors[j].max_capacity) {
                int score = calculateCompatibilityScore(&students[i], &mentors[j]);

                /* Mentor must have a positive compatibility score to be eligible */
                if (score > 0) {
                    if (score > max_score) {
                        max_score = score;
                        best_mentor_index = j;
                    } else if (score == max_score && best_mentor_index != -1) {
                        /* Tie-breaker: choose mentor with fewer assigned students */
                        if (mentors[j].currently_assigned < mentors[best_mentor_index].currently_assigned) {
                            best_mentor_index = j;
                        }
                    }
                }
            }
        }

        /* Update allocation results */
        if (best_mentor_index != -1) {
            students[i].is_allocated = 1;
            students[i].allocated_mentor_id = mentors[best_mentor_index].id;
            strncpy(students[i].allocated_mentor_name, mentors[best_mentor_index].name, STR_LEN - 1);
            students[i].allocated_mentor_name[STR_LEN - 1] = '\0';
            students[i].compatibility_score = max_score;

            /* Immediately increment mentor workload */
            mentors[best_mentor_index].currently_assigned++;
            allocated_count++;
        } else {
            /* No mentor available or score is 0 */
            students[i].is_allocated = 0;
            students[i].allocated_mentor_id = -1;
            strcpy(students[i].allocated_mentor_name, "Not Available");
            students[i].compatibility_score = 0;
            unallocated_count++;
        }
    }

    is_allocation_done = 1;

    printf("[+] Allocation process completed successfully!\n");
    printf("    - Total Students Processed : %d\n", student_count);
    printf("    - Successfully Allocated   : %d\n", allocated_count);
    printf("    - Unallocated (Full/No Fit): %d\n", unallocated_count);
    printf("------------------------------------------------------------------------\n");
    printf("Use Option 8 to view detailed results or Option 9 to view mentor workload.\n");
}

/* ----------------------------------------------------------------------------
 * 8. DISPLAY ALLOCATION RESULTS
 * ---------------------------------------------------------------------------- */
void displayAllocationResults(void) {
    if (student_count == 0) {
        printf("\n[!] No students registered in the system.\n");
        return;
    }

    if (!is_allocation_done) {
        printf("\n[!] Mentors have not been allocated yet.\n");
        printf("    Please choose Option 7 (Allocate Mentors to Students) first.\n");
        return;
    }

    printf("\n========================================================================\n");
    printf("                 STUDENT MENTORSHIP ALLOCATION RESULTS                  \n");
    printf("========================================================================\n");
    printf("%-12s %-16s %-18s %-8s %-15s\n",
           "Student ID", "Student Name", "Assigned Mentor", "Score", "Status");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < student_count; i++) {
        const char *status_str = students[i].is_allocated ? "Allocated" : "Not Allocated";
        printf("%-12d %-16s %-18s %-8d %-15s\n",
               students[i].id,
               students[i].name,
               students[i].allocated_mentor_name,
               students[i].compatibility_score,
               status_str);
    }

    printf("========================================================================\n");
}

/* ----------------------------------------------------------------------------
 * 9. DISPLAY MENTOR WORKLOAD
 * ---------------------------------------------------------------------------- */
void displayMentorWorkload(void) {
    if (mentor_count == 0) {
        printf("\n[!] No mentors registered in the system.\n");
        return;
    }

    printf("\n=========================================================================================\n");
    printf("                                   MENTOR WORKLOAD REPORT                                \n");
    printf("=========================================================================================\n");
    printf("%-10s %-18s %-12s %-10s %-10s %-12s %-18s\n",
           "Mentor ID", "Mentor Name", "Department", "Capacity", "Assigned", "Available", "Utilization");
    printf("-----------------------------------------------------------------------------------------\n");

    int total_capacity = 0;
    int total_assigned = 0;

    for (int i = 0; i < mentor_count; i++) {
        int avail = mentors[i].max_capacity - mentors[i].currently_assigned;
        int pct = (mentors[i].max_capacity > 0)
                  ? (mentors[i].currently_assigned * 100) / mentors[i].max_capacity
                  : 0;

        char util_str[30];
        if (avail == 0) {
            snprintf(util_str, sizeof(util_str), "%3d%% [FULL]", pct);
        } else {
            snprintf(util_str, sizeof(util_str), "%3d%% (%d slots left)", pct, avail);
        }

        printf("%-10d %-18s %-12s %-10d %-10d %-12d %-18s\n",
               mentors[i].id,
               mentors[i].name,
               mentors[i].department,
               mentors[i].max_capacity,
               mentors[i].currently_assigned,
               avail,
               util_str);

        total_capacity += mentors[i].max_capacity;
        total_assigned += mentors[i].currently_assigned;
    }

    printf("=========================================================================================\n");
    printf("Total Mentor Capacity: %d | Total Assigned: %d | Total Free Slots: %d\n",
           total_capacity, total_assigned, total_capacity - total_assigned);
}

/* ----------------------------------------------------------------------------
 * 10. SORT STUDENTS BY ID (Bubble Sort)
 * ---------------------------------------------------------------------------- */
void sortStudentsById(void) {
    if (student_count <= 1) {
        if (student_count == 0) {
            printf("\n[!] No students to sort.\n");
        } else {
            printf("\n[*] Only 1 student in the system. The list is already sorted.\n");
            displayAllStudents();
        }
        return;
    }

    printf("\n========================================================================\n");
    printf("                  SORTING STUDENTS BY ID (BUBBLE SORT)                  \n");
    printf("========================================================================\n");
    printf("Algorithm       : Bubble Sort\n");
    printf("Operation       : Repeatedly steps through list, compares adjacent items\n");
    printf("                  and swaps them if in wrong order.\n");
    printf("Time Complexity : Best Case O(N), Average/Worst Case O(N^2)\n");
    printf("Space Complexity: O(1) Auxiliary In-place sorting\n");
    printf("------------------------------------------------------------------------\n");

    int passes = 0;
    int swaps = 0;

    /* Bubble Sort Algorithm with swapped optimization */
    for (int i = 0; i < student_count - 1; i++) {
        int swapped = 0;
        passes++;
        for (int j = 0; j < student_count - 1 - i; j++) {
            if (students[j].id > students[j + 1].id) {
                /* Swap students[j] and students[j + 1] */
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
                swapped = 1;
                swaps++;
            }
        }
        /* If no two elements were swapped in this pass, array is sorted */
        if (!swapped) {
            break;
        }
    }

    printf("[+] Sorting completed successfully in %d pass(es) with %d total swap(s).\n\n",
           passes, swaps);

    /* Display the sorted array */
    displayAllStudents();
}

/* ----------------------------------------------------------------------------
 * 11. RESET ALLOCATIONS
 * ---------------------------------------------------------------------------- */
void resetAllocations(void) {
    if (student_count == 0 && mentor_count == 0) {
        printf("\n[!] No students or mentors registered in the system to reset.\n");
        return;
    }

    /* Reset student allocation attributes */
    for (int i = 0; i < student_count; i++) {
        students[i].is_allocated = 0;
        students[i].allocated_mentor_id = -1;
        strcpy(students[i].allocated_mentor_name, "Not Available");
        students[i].compatibility_score = 0;
    }

    /* Reset mentor assigned counts */
    for (int j = 0; j < mentor_count; j++) {
        mentors[j].currently_assigned = 0;
    }

    is_allocation_done = 0;

    printf("\n[+] Success: All student allocations have been cleared!\n");
    printf("    - Student allocation statuses reset to 'Not Allocated'\n");
    printf("    - All mentor workloads reset to 0\n");
    printf("    You can now re-run Option 7 (Allocate) or add/modify records.\n");
}

/* ----------------------------------------------------------------------------
 * LOAD SAMPLE DATA
 * ---------------------------------------------------------------------------- */
void loadSampleData(void) {
    student_count = 0;
    mentor_count = 0;
    is_allocation_done = 0;

    /* --- Add 3 Mentors --- */

    /* Mentor 1: Amit - CSE, Machine Learning, Capacity: 2 */
    mentors[0].id = 201;
    strncpy(mentors[0].name, "Amit", STR_LEN);
    strncpy(mentors[0].department, "CSE", STR_LEN);
    strncpy(mentors[0].expertise, "Machine Learning", STR_LEN);
    mentors[0].max_capacity = 2;
    mentors[0].currently_assigned = 0;

    /* Mentor 2: Neha - IT, Web Development, Capacity: 1 (Tight Capacity) */
    mentors[1].id = 202;
    strncpy(mentors[1].name, "Neha", STR_LEN);
    strncpy(mentors[1].department, "IT", STR_LEN);
    strncpy(mentors[1].expertise, "Web Development", STR_LEN);
    mentors[1].max_capacity = 1;
    mentors[1].currently_assigned = 0;

    /* Mentor 3: Rajesh - ECE, Embedded Systems, Capacity: 1 */
    mentors[2].id = 203;
    strncpy(mentors[2].name, "Rajesh", STR_LEN);
    strncpy(mentors[2].department, "ECE", STR_LEN);
    strncpy(mentors[2].expertise, "Embedded Systems", STR_LEN);
    mentors[2].max_capacity = 1;
    mentors[2].currently_assigned = 0;

    mentor_count = 3;

    /* --- Add 5 Students --- */

    /* Student 1: Rahul - ECE, Domain: Machine Learning, Skill: Machine Learning
     * Matches Amit: Domain(50) + Skill(30) + Dept(0) = 80 Points
     */
    students[0].id = 101;
    strncpy(students[0].name, "Rahul", STR_LEN);
    strncpy(students[0].department, "ECE", STR_LEN);
    strncpy(students[0].preferred_domain, "Machine Learning", STR_LEN);
    strncpy(students[0].technical_skill, "Machine Learning", STR_LEN);
    students[0].is_allocated = 0;
    students[0].allocated_mentor_id = -1;
    strcpy(students[0].allocated_mentor_name, "Not Available");
    students[0].compatibility_score = 0;

    /* Student 2: Priya - IT, Domain: Web Development, Skill: Web Development
     * Matches Neha: Domain(50) + Skill(30) + Dept(20) = 100 Points (Full Match)
     */
    students[1].id = 102;
    strncpy(students[1].name, "Priya", STR_LEN);
    strncpy(students[1].department, "IT", STR_LEN);
    strncpy(students[1].preferred_domain, "Web Development", STR_LEN);
    strncpy(students[1].technical_skill, "Web Development", STR_LEN);
    students[1].is_allocated = 0;
    students[1].allocated_mentor_id = -1;
    strcpy(students[1].allocated_mentor_name, "Not Available");
    students[1].compatibility_score = 0;

    /* Student 3: Aman - Mechanical, Domain: Robotics, Skill: CAD Design
     * Matches No Mentors: Domain(0) + Skill(0) + Dept(0) = 0 Points -> Not Allocated
     */
    students[2].id = 103;
    strncpy(students[2].name, "Aman", STR_LEN);
    strncpy(students[2].department, "Mechanical", STR_LEN);
    strncpy(students[2].preferred_domain, "Robotics", STR_LEN);
    strncpy(students[2].technical_skill, "CAD Design", STR_LEN);
    students[2].is_allocated = 0;
    students[2].allocated_mentor_id = -1;
    strcpy(students[2].allocated_mentor_name, "Not Available");
    students[2].compatibility_score = 0;

    /* Student 4: Sneha - IT, Domain: Web Development, Skill: HTML/CSS
     * Wants Neha (Score 70), BUT Neha only has capacity 1, taken by Priya!
     * Demonstrates Mentor Capacity Saturation -> Not Allocated
     */
    students[3].id = 104;
    strncpy(students[3].name, "Sneha", STR_LEN);
    strncpy(students[3].department, "IT", STR_LEN);
    strncpy(students[3].preferred_domain, "Web Development", STR_LEN);
    strncpy(students[3].technical_skill, "HTML/CSS", STR_LEN);
    students[3].is_allocated = 0;
    students[3].allocated_mentor_id = -1;
    strcpy(students[3].allocated_mentor_name, "Not Available");
    students[3].compatibility_score = 0;

    /* Student 5: Vikram - CSE, Domain: Machine Learning, Skill: Python
     * Matches Amit: Domain(50) + Skill(0) + Dept(20) = 70 Points
     * Fills Amit's remaining capacity (2/2) -> Allocated
     */
    students[4].id = 105;
    strncpy(students[4].name, "Vikram", STR_LEN);
    strncpy(students[4].department, "CSE", STR_LEN);
    strncpy(students[4].preferred_domain, "Machine Learning", STR_LEN);
    strncpy(students[4].technical_skill, "Python", STR_LEN);
    students[4].is_allocated = 0;
    students[4].allocated_mentor_id = -1;
    strcpy(students[4].allocated_mentor_name, "Not Available");
    students[4].compatibility_score = 0;

    student_count = 5;

    printf("\n[+] Sample demonstration dataset loaded successfully!\n");
    printf("    - 3 Mentors (IDs: 201 [Amit], 202 [Neha], 203 [Rajesh])\n");
    printf("    - 5 Students (IDs: 101 [Rahul], 102 [Priya], 103 [Aman], 104 [Sneha], 105 [Vikram])\n");
    printf("    Tip: Use Option 2 and Option 4 to view loaded students and mentors,\n");
    printf("    then run Option 7 (Allocate) to observe the intelligent matching.\n");
}

/* ----------------------------------------------------------------------------
 * HELPER / UTILITY FUNCTIONS
 * ---------------------------------------------------------------------------- */

/* Find index of student by ID using Linear Search (-1 if not found) */
int findStudentIndexById(int id) {
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Find index of mentor by ID using Linear Search (-1 if not found) */
int findMentorIndexById(int id) {
    for (int i = 0; i < mentor_count; i++) {
        if (mentors[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* Case-insensitive string equality comparison */
int stringEqualsIgnoreCase(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL) return 0;
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return 0;
        }
        s1++;
        s2++;
    }
    return (*s1 == '\0' && *s2 == '\0');
}

/* Case-insensitive substring check: returns 1 if needle is inside haystack */
int stringContainsIgnoreCase(const char *haystack, const char *needle) {
    if (haystack == NULL || needle == NULL || *needle == '\0') return 0;

    int h_len = (int)strlen(haystack);
    int n_len = (int)strlen(needle);
    if (n_len > h_len) return 0;

    for (int i = 0; i <= h_len - n_len; i++) {
        int match = 1;
        for (int j = 0; j < n_len; j++) {
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

/* General match function: checks exact or substring match case-insensitively */
int stringMatch(const char *s1, const char *s2) {
    if (s1 == NULL || s2 == NULL || strlen(s1) == 0 || strlen(s2) == 0) return 0;
    if (stringEqualsIgnoreCase(s1, s2)) return 1;
    if (stringContainsIgnoreCase(s2, s1) || stringContainsIgnoreCase(s1, s2)) return 1;
    return 0;
}

/* Calculate Compatibility Score between a Student and Mentor */
int calculateCompatibilityScore(const Student *s, const Mentor *m) {
    int score = 0;

    /* 1. Preferred Mentoring Domain Match : 50 points */
    if (stringMatch(s->preferred_domain, m->expertise)) {
        score += 50;
    }

    /* 2. Technical Skill Match : 30 points */
    if (stringMatch(s->technical_skill, m->expertise)) {
        score += 30;
    }

    /* 3. Department Match : 20 points */
    if (stringEqualsIgnoreCase(s->department, m->department)) {
        score += 20;
    }

    return score;
}

/* Safe string reader using fgets, automatically trims trailing newline */
void readString(const char *prompt, char *buffer, int max_len) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, max_len, stdin) != NULL) {
            size_t len = strlen(buffer);
            int newline_found = 0;
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
                len--;
                newline_found = 1;
            }
            /* If buffer was filled without a newline, discard remaining characters on that line */
            if (!newline_found && len == (size_t)(max_len - 1)) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF);
            }
            if (len > 0) {
                break;
            }
        }
        printf("[!] Field cannot be empty. Please enter a valid value.\n");
    }
}

/* Robust integer reader that handles invalid non-numeric inputs cleanly */
int readInteger(const char *prompt) {
    char line[64];
    int value;

    while (1) {
        if (prompt != NULL && strlen(prompt) > 0) {
            printf("%s", prompt);
        }
        if (fgets(line, sizeof(line), stdin) != NULL) {
            /* Try parsing an integer */
            if (sscanf(line, "%d", &value) == 1) {
                return value;
            }
        }
        printf("[!] Invalid input. Please enter a valid number: ");
    }
}
