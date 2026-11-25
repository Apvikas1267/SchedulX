import sys
import os
from pptx import Presentation
from pptx.util import Inches, Pt
from pptx.dml.color import RGBColor
from pptx.enum.text import PP_ALIGN, MSO_ANCHOR
from pptx.enum.shapes import MSO_SHAPE

def create_deck():
    prs = Presentation()
    prs.slide_width = Inches(13.333)
    prs.slide_height = Inches(7.5)
    blank_layout = prs.slide_layouts[6]

    # Color Palette matching the RVCE template
    C_BG = RGBColor(248, 249, 250)         # Soft off-white
    C_CARD = RGBColor(255, 255, 255)       # Pure white
    C_PRIMARY = RGBColor(26, 37, 48)       # Deep slate navy
    C_SECONDARY = RGBColor(51, 65, 85)     # Slate text
    C_ACCENT = RGBColor(37, 99, 235)       # Academic Royal Blue
    C_ACCENT_BG = RGBColor(238, 242, 255)  # Light blue tint
    C_BORDER = RGBColor(226, 232, 240)     # Subtle border
    C_GREEN = RGBColor(22, 163, 74)        # Success green
    C_BOX_BG = RGBColor(241, 245, 249)     # Soft grey box
    C_MUTED = RGBColor(100, 116, 139)      # Muted grey

    def add_header(slide, title_text, category_text="Experiential Learning"):
        # Header category
        cat_box = slide.shapes.add_textbox(Inches(0.8), Inches(0.4), Inches(11), Inches(0.35))
        tf_cat = cat_box.text_frame
        tf_cat.word_wrap = True
        tf_cat.margin_left = tf_cat.margin_right = tf_cat.margin_top = tf_cat.margin_bottom = 0
        p_cat = tf_cat.paragraphs[0]
        p_cat.text = category_text.upper()
        p_cat.font.size = Pt(11)
        p_cat.font.bold = True
        p_cat.font.color.rgb = C_ACCENT
        p_cat.font.name = "Arial"

        # Main Slide Title
        title_box = slide.shapes.add_textbox(Inches(0.8), Inches(0.7), Inches(11), Inches(0.6))
        tf_title = title_box.text_frame
        tf_title.word_wrap = True
        tf_title.margin_left = tf_title.margin_right = tf_title.margin_top = tf_title.margin_bottom = 0
        p_title = tf_title.paragraphs[0]
        p_title.text = title_text
        p_title.font.size = Pt(24)
        p_title.font.bold = True
        p_title.font.color.rgb = C_PRIMARY
        p_title.font.name = "Arial"

        # Divider line
        line = slide.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.8), Inches(1.35), Inches(11.733), Inches(0.02))
        line.fill.solid()
        line.fill.fore_color.rgb = C_BORDER
        line.line.color.rgb = C_BORDER

    # ==========================================
    # SLIDE 1: TITLE SLIDE
    # ==========================================
    s1 = prs.slides.add_slide(blank_layout)
    bg1 = s1.shapes.add_shape(MSO_SHAPE.RECTANGLE, 0, 0, Inches(13.333), Inches(7.5))
    bg1.fill.solid()
    bg1.fill.fore_color.rgb = RGBColor(250, 252, 255)
    bg1.line.fill.background()

    # Top institutional badge
    inst_box = s1.shapes.add_textbox(Inches(1.0), Inches(0.8), Inches(11.333), Inches(0.5))
    tf_inst = inst_box.text_frame
    p_inst = tf_inst.paragraphs[0]
    p_inst.text = "R V COLLEGE OF ENGINEERING ® • DEPARTMENT OF COMPUTER SCIENCE & ENGINEERING"
    p_inst.font.size = Pt(12)
    p_inst.font.bold = True
    p_inst.font.color.rgb = C_ACCENT
    p_inst.font.name = "Arial"

    sub_inst = tf_inst.add_paragraph()
    sub_inst.text = "CS233AI: Data Structures and Applications • Experiential Learning (DSA EL)"
    sub_inst.font.size = Pt(11)
    sub_inst.font.color.rgb = C_MUTED
    sub_inst.font.name = "Arial"

    # Title Card container
    card1 = s1.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(1.0), Inches(1.7), Inches(11.333), Inches(3.2))
    card1.fill.solid()
    card1.fill.fore_color.rgb = C_PRIMARY
    card1.line.fill.background()

    tf_c1 = card1.text_frame
    tf_c1.word_wrap = True
    tf_c1.vertical_anchor = MSO_ANCHOR.MIDDLE

    p_t1 = tf_c1.paragraphs[0]
    p_t1.text = "SchedulX"
    p_t1.font.size = Pt(44)
    p_t1.font.bold = True
    p_t1.font.color.rgb = RGBColor(255, 255, 255)
    p_t1.font.name = "Arial"
    p_t1.alignment = PP_ALIGN.CENTER

    p_sub = tf_c1.add_paragraph()
    p_sub.text = "DSA-Based CPU Scheduling & Process Management Simulator"
    p_sub.font.size = Pt(20)
    p_sub.font.bold = True
    p_sub.font.color.rgb = RGBColor(147, 197, 253)
    p_sub.font.name = "Arial"
    p_sub.alignment = PP_ALIGN.CENTER

    p_desc = tf_c1.add_paragraph()
    p_desc.text = "A hand-crafted pure C simulation engine implementing 7 foundational data structures without built-in libraries,\ncoupled with an interactive web visualizer and public cloud deployment."
    p_desc.font.size = Pt(12)
    p_desc.font.color.rgb = RGBColor(203, 213, 225)
    p_desc.font.name = "Arial"
    p_desc.alignment = PP_ALIGN.CENTER

    # Student metadata cards below
    meta_card = s1.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(1.0), Inches(5.1), Inches(11.333), Inches(1.7))
    meta_card.fill.solid()
    meta_card.fill.fore_color.rgb = C_CARD
    meta_card.line.color.rgb = C_BORDER
    meta_card.line.width = Pt(1.5)

    tf_m = meta_card.text_frame
    tf_m.word_wrap = True
    p_m1 = tf_m.paragraphs[0]
    p_m1.text = "PROJECT DETAILS & TEAM"
    p_m1.font.size = Pt(11)
    p_m1.font.bold = True
    p_m1.font.color.rgb = C_ACCENT
    p_m1.font.name = "Arial"

    p_m2 = tf_m.add_paragraph()
    p_m2.text = "Student Name: AP Vikas                                                   Course: Data Structures and Applications (CS233AI)"
    p_m2.font.size = Pt(13)
    p_m2.font.bold = True
    p_m2.font.color.rgb = C_PRIMARY
    p_m2.font.name = "Arial"

    p_m3 = tf_m.add_paragraph()
    p_m3.text = "USN: [Your USN Here]                                                    Repository: github.com/Apvikas1267/SchedulX"
    p_m3.font.size = Pt(12)
    p_m3.font.color.rgb = C_SECONDARY
    p_m3.font.name = "Arial"

    p_m4 = tf_m.add_paragraph()
    p_m4.text = "Base Paper: S. Suranauwarat, 'Using an Interactive Animated Tool to Improve the Effectiveness of Learning CPU Scheduling Algorithms', IEEE (2015)"
    p_m4.font.size = Pt(10)
    p_m4.font.color.rgb = C_MUTED
    p_m4.font.name = "Arial"

    # ==========================================
    # SLIDE 2: PRESENTATION OUTLINE
    # ==========================================
    s2 = prs.slides.add_slide(blank_layout)
    add_header(s2, "Presentation Outline")

    items = [
        ("01", "Introduction", "Overview of SchedulX, pure C implementation & core simulation pipeline"),
        ("02", "Problem Statement", "Textbook abstractions, black-box schedulers & dependency deadlocks"),
        ("03", "Objectives", "5 primary milestones: algorithms, custom DSA, DAGs, visualizer & cloud"),
        ("04", "Methodology & Architecture", "End-to-end execution flow: from process store to Gantt timeline"),
        ("05", "Data Structures Used", "Comprehensive mapping of 7 hand-crafted data structures without built-ins"),
        ("06", "Results & Verification", "622 test cases, 12,000 random verification runs & textbook benchmark"),
        ("07", "Conclusion & Future Scope", "Key achievements, syllabus mapping, limitations and extensions"),
        ("08", "References", "IEEE base paper, standard literature and algorithm textbooks")
    ]

    for idx, (num, title, desc) in enumerate(items):
        col = idx % 2
        row = idx // 2
        x = Inches(0.8 + col * 5.95)
        y = Inches(1.6 + row * 1.35)

        card = s2.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, x, y, Inches(5.75), Inches(1.15))
        card.fill.solid()
        card.fill.fore_color.rgb = C_CARD
        card.line.color.rgb = C_BORDER
        card.line.width = Pt(1)

        # Number pill
        pill = s2.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, x + Inches(0.18), y + Inches(0.22), Inches(0.7), Inches(0.7))
        pill.fill.solid()
        pill.fill.fore_color.rgb = C_ACCENT_BG
        pill.line.fill.background()
        tf_p = pill.text_frame
        p_p = tf_p.paragraphs[0]
        p_p.text = num
        p_p.font.size = Pt(16)
        p_p.font.bold = True
        p_p.font.color.rgb = C_ACCENT
        p_p.alignment = PP_ALIGN.CENTER

        # Text box
        tbox = s2.shapes.add_textbox(x + Inches(1.05), y + Inches(0.15), Inches(4.5), Inches(0.85))
        tf_t = tbox.text_frame
        tf_t.word_wrap = True
        p_t = tf_t.paragraphs[0]
        p_t.text = title
        p_t.font.size = Pt(14)
        p_t.font.bold = True
        p_t.font.color.rgb = C_PRIMARY

        p_d = tf_t.add_paragraph()
        p_d.text = desc
        p_d.font.size = Pt(10.5)
        p_d.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 3: INTRODUCTION
    # ==========================================
    s3 = prs.slides.add_slide(blank_layout)
    add_header(s3, "Introduction")

    intro_points = [
        ("High-Performance Pure C Architecture",
         "SchedulX is a comprehensive CPU scheduling and process management simulator whose entire scheduling logic and data structures are written strictly in pure C (C99/GNU11). JavaScript only collects user input and renders graphs; all scheduling calculations are performed by C structures."),
        ("End-to-End Core Pipeline",
         "The pipeline seamlessly connects: Process Store (Linked List) → PID Index (DJB2 Hash Table) → Precedence Resolver (DAG Topological Sort) → Ready Queue Dispatcher (FIFO Queue / Circular Queue / Min-Heap) → Execution Engine → History Trace (LIFO Stack) → Performance Stats."),
        ("Six Classical Scheduling Algorithms Supported",
         "Implements First-Come, First-Served (FCFS), Shortest Job First (non-preemptive), Shortest Remaining Time First (preemptive), Round Robin (configurable quantum), and Priority Scheduling (both preemptive and non-preemptive modes with customizable priority direction)."),
        ("Strict 'No Built-in Function' Compliance",
         "Every data structure—Singly Linked List, Separate-Chaining Hash Table, FIFO Queue, Circular Queue, Binary Min-Heap, Stack, and Graph Adjacency List—is built from scratch. Standard library qsort was replaced with a hand-written insertion sort for arrival tie-breaking."),
        ("Dual-Mode Deployment: Local & Cloud",
         "Runs natively on desktop via a lightweight embedded C Winsock HTTP server (localhost:8080) for grading/viva, and is concurrently deployed live to the public internet on Vercel via serverless execution with zero external dependencies.")
    ]

    for idx, (title, body) in enumerate(intro_points):
        y = Inches(1.6 + idx * 1.08)
        card = s3.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), y, Inches(11.733), Inches(0.95))
        card.fill.solid()
        card.fill.fore_color.rgb = C_CARD
        card.line.color.rgb = C_BORDER
        card.line.width = Pt(1)

        # bullet pill
        pill = s3.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.8), y, Inches(0.12), Inches(0.95))
        pill.fill.solid()
        pill.fill.fore_color.rgb = C_ACCENT
        pill.line.fill.background()

        tbox = s3.shapes.add_textbox(Inches(1.1), y + Inches(0.08), Inches(11.2), Inches(0.8))
        tf = tbox.text_frame
        tf.word_wrap = True
        p1 = tf.paragraphs[0]
        p1.text = title
        p1.font.size = Pt(13)
        p1.font.bold = True
        p1.font.color.rgb = C_PRIMARY

        p2 = tf.add_paragraph()
        p2.text = body
        p2.font.size = Pt(10.5)
        p2.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 4: PROBLEM STATEMENT
    # ==========================================
    s4 = prs.slides.add_slide(blank_layout)
    add_header(s4, "Problem Statement")

    probs = [
        ("Theoretical Textbook Abstraction",
         "Standard operating systems textbooks illustrate CPU scheduling through static, hand-calculated tables and simplified execution models. Students cannot observe how dynamic burst arrivals, preemptions, and context switches happen in a real-time running system."),
        ("Black-Box Scheduler Disconnect",
         "Existing educational tools treat schedulers as 'black boxes'—they compute waiting time but hide the underlying data structures. Learners cannot see how a circular queue wraps around, how a min-heap restores its invariant, or how hash buckets resolve collisions."),
        ("Unresolved Dependencies & Deadlock Risk",
         "In real workloads, processes have strict execution precedence (e.g., Process B requires Process A's output). Classical simulators ignore dependencies, risking race conditions and circular deadlocks when processes wait on each other."),
        ("Lack of Comparative Benchmarking",
         "Evaluating which algorithm performs best under specific conditions (burst variation, arrival clustering, tiny vs. large quantum) is difficult without running all algorithms on the exact same workload with standardized comparative metrics."),
        ("Need for First-Principles C Implementation",
         "Higher-level languages (Python, Java) hide memory allocations and pointer operations behind automated garbage collection and built-in classes. A pure C implementation with zero built-in functions is essential to master true low-level systems engineering.")
    ]

    for idx, (title, desc) in enumerate(probs):
        y = Inches(1.6 + idx * 1.08)
        card = s4.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), y, Inches(11.733), Inches(0.95))
        card.fill.solid()
        card.fill.fore_color.rgb = C_CARD
        card.line.color.rgb = C_BORDER
        card.line.width = Pt(1)

        pill = s4.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.8), y, Inches(0.12), Inches(0.95))
        pill.fill.solid()
        pill.fill.fore_color.rgb = RGBColor(225, 29, 72) # Red/crimson alert
        pill.line.fill.background()

        tbox = s4.shapes.add_textbox(Inches(1.1), y + Inches(0.08), Inches(11.2), Inches(0.8))
        tf = tbox.text_frame
        tf.word_wrap = True
        p1 = tf.paragraphs[0]
        p1.text = title
        p1.font.size = Pt(13)
        p1.font.bold = True
        p1.font.color.rgb = C_PRIMARY

        p2 = tf.add_paragraph()
        p2.text = desc
        p2.font.size = Pt(10.5)
        p2.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 5: OBJECTIVES
    # ==========================================
    s5 = prs.slides.add_slide(blank_layout)
    add_header(s5, "Objectives")

    objs = [
        ("1. Implement Core CPU Scheduling Algorithms in C",
         "Develop robust, standalone C implementations of FCFS, Non-Preemptive SJF, Preemptive SRTF, Round Robin, and Priority Scheduling (both preemptive and non-preemptive), strictly following syllabus tie-breaking conventions."),
        ("2. Build Foundational Data Structures from Scratch",
         "Hand-craft all seven course data structures in pure C without relying on built-in libraries: Singly Linked List, Separate-Chaining Hash Table, FIFO Queue, Circular Queue, Binary Min-Heap, LIFO Stack, and Directed Graph."),
        ("3. Precedence Management & Deadlock Prevention via DAGs",
         "Implement a Directed Acyclic Graph (DAG) using adjacency lists, Kahn's Topological Sort algorithm for dependency scheduling, and 3-color DFS cycle detection to reject deadlocked processes before simulation."),
        ("4. Interactive Visualization & Step-by-Step Tracing",
         "Provide a rich interactive frontend featuring dynamic Gantt charts, context-switch preemption flags, an execution timeline slider, and live data structure inspectors showing ready structures after every CPU dispatch."),
        ("5. Quantitative Benchmarking & Dual Deployment",
         "Compute exact metrics (Turnaround Time, Waiting Time, Response Time, CPU Utilization, Throughput), support multi-algorithm side-by-side comparison, and provide dual deployment (Local C Server & Live Vercel Public Link).")
    ]

    for idx, (title, desc) in enumerate(objs):
        y = Inches(1.6 + idx * 1.08)
        card = s5.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), y, Inches(11.733), Inches(0.95))
        card.fill.solid()
        card.fill.fore_color.rgb = C_CARD
        card.line.color.rgb = C_BORDER
        card.line.width = Pt(1)

        pill = s5.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.8), y, Inches(0.12), Inches(0.95))
        pill.fill.solid()
        pill.fill.fore_color.rgb = C_GREEN
        pill.line.fill.background()

        tbox = s5.shapes.add_textbox(Inches(1.1), y + Inches(0.08), Inches(11.2), Inches(0.8))
        tf = tbox.text_frame
        tf.word_wrap = True
        p1 = tf.paragraphs[0]
        p1.text = title
        p1.font.size = Pt(13)
        p1.font.bold = True
        p1.font.color.rgb = C_PRIMARY

        p2 = tf.add_paragraph()
        p2.text = desc
        p2.font.size = Pt(10.5)
        p2.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 6: METHODOLOGY & ARCHITECTURE
    # ==========================================
    s6 = prs.slides.add_slide(blank_layout)
    add_header(s6, "Methodology & System Architecture")

    # Left Flowchart (Pipeline)
    flow_steps = [
        ("Input Dataset", "User enters PIDs, Arrival, Burst, Priority, Dependencies (or CSV/sample)"),
        ("Process Store & Hash Index", "Stored in Singly Linked List; indexed via DJB2 Hash Table for O(1) PID lookup"),
        ("Precedence Analyzer (DAG)", "Topological Sort (Kahn's) orders eligible jobs; DFS cycle detection checks deadlocks"),
        ("Ready Queue Dispatcher", "FCFS: Linked Queue | RR: Circular Queue | SJF/SRTF/Priority: Binary Min-Heap"),
        ("Execution Engine", "Simulates time ticks, handles quantum slices, triggers preemption, records Gantt segments"),
        ("History Trace & Metrics", "LIFO Stack records dispatches; computes Completion, Turnaround, Waiting, CPU Util %"),
        ("Interactive Frontend", "Serves web GUI via C Winsock / Vercel Serverless; renders Gantt & DSA inspectors")
    ]

    for idx, (title, desc) in enumerate(flow_steps):
        y = Inches(1.6 + idx * 0.77)
        box = s6.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), y, Inches(7.5), Inches(0.68))
        box.fill.solid()
        box.fill.fore_color.rgb = C_BOX_BG
        box.line.color.rgb = C_BORDER
        box.line.width = Pt(1)

        tbox = box.text_frame
        tbox.word_wrap = True
        p1 = tbox.paragraphs[0]
        p1.text = f"{idx+1}. {title}: "
        p1.font.size = Pt(11.5)
        p1.font.bold = True
        p1.font.color.rgb = C_PRIMARY

        run = p1.add_run()
        run.text = desc
        run.font.bold = False
        run.font.size = Pt(10)
        run.font.color.rgb = C_SECONDARY

    # Right Box: "Data Structures Updated for Every Dispatch"
    side_card = s6.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(8.6), Inches(1.6), Inches(3.933), Inches(5.25))
    side_card.fill.solid()
    side_card.fill.fore_color.rgb = C_CARD
    side_card.line.color.rgb = C_ACCENT
    side_card.line.width = Pt(1.5)

    tf_s = side_card.text_frame
    tf_s.word_wrap = True
    p_st = tf_s.paragraphs[0]
    p_st.text = "Data Structures Role"
    p_st.font.size = Pt(14)
    p_st.font.bold = True
    p_st.font.color.rgb = C_ACCENT

    p_sub = tf_s.add_paragraph()
    p_sub.text = "Updated for every CPU dispatch"
    p_sub.font.size = Pt(10)
    p_sub.font.color.rgb = C_MUTED

    ds_boxes = [
        ("Hash Table (djb2)", "O(1) average lookup by PID; separate chaining for collisions."),
        ("Linked List", "Dynamic process store with O(1) head/tail insertion."),
        ("Binary Min-Heap", "O(log n) extract-min on burst, remaining time, or priority."),
        ("Circular Queue", "Modulo wrap-around array queue for Round Robin rotation."),
        ("LIFO Stack", "Maintains ordered CPU execution dispatch history trace."),
        ("DAG (Adjacency List)", "Tracks in-degrees; resolves inter-process dependencies.")
    ]

    for d_idx, (d_title, d_desc) in enumerate(ds_boxes):
        d_y = Inches(2.35 + d_idx * 0.72)
        d_shape = s6.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(8.8), d_y, Inches(3.533), Inches(0.62))
        d_shape.fill.solid()
        d_shape.fill.fore_color.rgb = C_BOX_BG
        d_shape.line.color.rgb = C_BORDER
        tf_d = d_shape.text_frame
        p_dt = tf_d.paragraphs[0]
        p_dt.text = d_title
        p_dt.font.size = Pt(11)
        p_dt.font.bold = True
        p_dt.font.color.rgb = C_PRIMARY

        p_dd = tf_d.add_paragraph()
        p_dd.text = d_desc
        p_dd.font.size = Pt(9.5)
        p_dd.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 7: DATA STRUCTURES USED (TABLE)
    # ==========================================
    s7 = prs.slides.add_slide(blank_layout)
    add_header(s7, "Data Structures Used")

    rows = [
        ("Concept", "Where It Is Used", "Why It Fits / Time Complexity & Rationale"),
        ("Singly Linked List", "Process management store (insert, edit, delete, search)", "Dynamic allocation; processes added/removed frequently without array shifting; O(1) tail insert."),
        ("Hash Table (djb2)", "O(1) PID lookup in process table and dependency validation", "Direct access by string PID; 13 buckets with separate chaining linked lists for collision resolution."),
        ("FIFO Queue (Linked)", "FCFS ready queue buffer", "Strict First-In-First-Out ordering; dynamic linked nodes prevent arbitrary capacity limits; O(1) enqueue/dequeue."),
        ("Circular Queue (Array)", "Round Robin ready queue", "Array with modulo arithmetic (front + size) % cap; enables efficient cyclic rotation of preempted slices."),
        ("Binary Min-Heap", "SJF, SRTF, and Priority ready queue", "Complete binary tree in array; O(log n) sift-up and sift-down; root guarantees immediate O(1) lowest burst/highest priority."),
        ("LIFO Stack", "CPU execution history trace", "Last-In-First-Out order; push on dispatch records execution sequence; allows step-by-step visualizer replay."),
        ("Directed Graph (DAG)", "Process dependency resolution (A -> B)", "Adjacency list stores precedence edges; Kahn's topological sort finds eligible tasks; DFS detects deadlocks."),
        ("Insertion Sort", "Arrival tie-breaker in scheduler", "In-place, stable O(n) sort on small candidate sets; eliminates standard library qsort() for 100% custom C compliance.")
    ]

    t_shape = s7.shapes.add_table(len(rows), 3, Inches(0.8), Inches(1.6), Inches(11.733), Inches(5.2))
    tbl = t_shape.table
    tbl.columns[0].width = Inches(2.4)
    tbl.columns[1].width = Inches(3.6)
    tbl.columns[2].width = Inches(5.733)

    for r_idx, row in enumerate(rows):
        for c_idx, val in enumerate(row):
            cell = tbl.cell(r_idx, c_idx)
            cell.text = val
            p = cell.text_frame.paragraphs[0]
            p.font.name = "Arial"
            if r_idx == 0:
                cell.fill.solid()
                cell.fill.fore_color.rgb = C_PRIMARY
                p.font.bold = True
                p.font.size = Pt(12)
                p.font.color.rgb = RGBColor(255, 255, 255)
            else:
                cell.fill.solid()
                cell.fill.fore_color.rgb = C_CARD if r_idx % 2 == 1 else C_BOX_BG
                p.font.size = Pt(10.5)
                if c_idx == 0:
                    p.font.bold = True
                    p.font.color.rgb = C_PRIMARY
                else:
                    p.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 8: RESULTS & VERIFICATION
    # ==========================================
    s8 = prs.slides.add_slide(blank_layout)
    add_header(s8, "Results & Verification")

    res_data = [
        ("Test Category / Scenario", "Observed Outcome & Verified Performance Metrics"),
        ("622 Automated Unit Checks (test_scheduler.c)", "100% passed (622/622). Verifies heap property invariants, circular queue wrap-around, stack LIFO, hash chaining, and list CRUD."),
        ("Textbook Benchmark (P1-P4: Bursts 8, 4, 9, 5)", "SRTF Avg Waiting Time = 6.50, Avg Turnaround = 13.00, CPU Util = 100%. Matches standard OS textbook results to the exact decimal."),
        ("Round Robin Quantum Sensitivity (q = 1, 2, 3, 4, 100)", "Higher quantum reduces context switches; q=1 produces 26 Gantt segments (AWT=12.50), while q=100 degenerates gracefully into FCFS (AWT=8.75)."),
        ("Simultaneous Arrivals at t = 2 (A, B, C, D)", "All jobs arrive together; tie-breaker preserves arrival/input order. SJF schedules shortest job C first; idle period [0-2] lowers CPU util to 83.33%."),
        ("Idle CPU Gap Handling", "CPU correctly jumps to future arrivals (e.g. t=4 to t=8) and generates explicit IDLE Gantt segments; total simulation time spans 0 to last completion."),
        ("Preemption & High-Burst Stress Test (500 units)", "P1 (burst 500) is preempted by short arrivals (P2, P3) under SRTF and preemptive priority; slices merge cleanly with zero memory leaks."),
        ("Cyclic Dependency Rejection (A -> B -> C -> A)", "Kahn's topological sort fails count; 3-color DFS traces and displays exact cycle path 'A -> B -> C -> A'; simulation safely refused without crashing."),
        ("12,000 Random Dataset Validation", "Compared tick-by-tick against an independent reference simulator across 12,000 randomized workloads: exactly 0 discrepancies observed.")
    ]

    t_res = s8.shapes.add_table(len(res_data), 2, Inches(0.8), Inches(1.6), Inches(11.733), Inches(5.2))
    tbl_r = t_res.table
    tbl_r.columns[0].width = Inches(4.5)
    tbl_r.columns[1].width = Inches(7.233)

    for r_idx, row in enumerate(res_data):
        for c_idx, val in enumerate(row):
            cell = tbl_r.cell(r_idx, c_idx)
            cell.text = val
            p = cell.text_frame.paragraphs[0]
            p.font.name = "Arial"
            if r_idx == 0:
                cell.fill.solid()
                cell.fill.fore_color.rgb = C_PRIMARY
                p.font.bold = True
                p.font.size = Pt(12)
                p.font.color.rgb = RGBColor(255, 255, 255)
            else:
                cell.fill.solid()
                cell.fill.fore_color.rgb = C_CARD if r_idx % 2 == 1 else C_BOX_BG
                p.font.size = Pt(10.5)
                if c_idx == 0:
                    p.font.bold = True
                    p.font.color.rgb = C_PRIMARY
                else:
                    p.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 9: CONCLUSION & FUTURE SCOPE
    # ==========================================
    s9 = prs.slides.add_slide(blank_layout)
    add_header(s9, "Conclusion & Future Scope")

    concl_points = [
        ("Full Syllabus Alignment & Zero Built-ins",
         "Successfully designed and implemented an experiential CPU scheduling simulator covering all CS233AI topics. All 7 core data structures and sorting algorithms are hand-coded in pure C without relying on C++ STL or standard libraries."),
        ("Rigorous Mathematical & Algorithmic Validation",
         "Demonstrated that min-heaps reduce priority extraction from O(n) to O(log n), circular queues ensure O(1) round-robin rotation, and hash tables guarantee O(1) process retrieval. Verified across 622 unit checks and 12,000 randomized test suites."),
        ("Interactive Learning via Visual State Tracing",
         "Bridged the textbook theory gap by visually depicting ready queues, heap trees, stack history, and Gantt charts after every dispatch, enabling students to inspect internal data structure states at any time tick."),
        ("Robust Dual-Mode Deployment",
         "Operates locally as a self-contained Winsock HTTP server for college evaluations, and is deployed live on Vercel via serverless architecture with an in-browser fallback engine ensuring 100% public uptime."),
        ("Future Scope & Enhancements",
         "Future extensions include multi-core symmetric multiprocessing (SMP) scheduling, Multilevel Feedback Queue (MLFQ) with dynamic aging, and real-time scheduling algorithms (Earliest Deadline First - EDF, Rate Monotonic - RMS).")
    ]

    for idx, (title, desc) in enumerate(concl_points):
        y = Inches(1.6 + idx * 1.08)
        card = s9.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), y, Inches(11.733), Inches(0.95))
        card.fill.solid()
        card.fill.fore_color.rgb = C_CARD
        card.line.color.rgb = C_BORDER
        card.line.width = Pt(1)

        pill = s9.shapes.add_shape(MSO_SHAPE.RECTANGLE, Inches(0.8), y, Inches(0.12), Inches(0.95))
        pill.fill.solid()
        pill.fill.fore_color.rgb = C_ACCENT
        pill.line.fill.background()

        tbox = s9.shapes.add_textbox(Inches(1.1), y + Inches(0.08), Inches(11.2), Inches(0.8))
        tf = tbox.text_frame
        tf.word_wrap = True
        p1 = tf.paragraphs[0]
        p1.text = title
        p1.font.size = Pt(13)
        p1.font.bold = True
        p1.font.color.rgb = C_PRIMARY

        p2 = tf.add_paragraph()
        p2.text = desc
        p2.font.size = Pt(10.5)
        p2.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 10: REFERENCES
    # ==========================================
    s10 = prs.slides.add_slide(blank_layout)
    add_header(s10, "References")

    refs = [
        ("[1] BASE PAPER: S. Suranauwarat, 'Using an Interactive Animated Tool to Improve the Effectiveness of Learning CPU Scheduling Algorithms,' 2015 IEEE Frontiers in Education Conference (FIE), El Paso, TX, USA, 2015, pp. 1-7, doi: 10.1109/FIE.2015.7344239."),
        ("[2] S. Suranauwarat, 'A CPU Scheduling Algorithm Simulator,' 37th Annual Frontiers in Education Conference - Global Engineering: Knowledge Without Borders, Opportunities Without Passports, Milwaukee, WI, USA, 2007, pp. T2A-1-T2A-5."),
        ("[3] A. Silberschatz, P. B. Galvin, and G. Gagne, Operating System Concepts, 10th ed., Hoboken, NJ: John Wiley & Sons, 2018."),
        ("[4] T. H. Cormen, C. E. Leiserson, R. L. Rivest, and C. Stein, Introduction to Algorithms, 4th ed., Cambridge, MA: MIT Press, 2022."),
        ("[5] B. W. Kernighan and D. M. Ritchie, The C Programming Language, 2nd ed., Englewood Cliffs, NJ: Prentice Hall, 1988."),
        ("[6] N. Goel and R. B. Garg, 'A Comparative Study of CPU Scheduling Algorithms,' International Journal of Graphics & Image Processing (IJGIP), vol. 2, no. 4, pp. 245-252, 2012 / arXiv:1208.5714."),
        ("[7] H. Topcuoglu, S. Hariri, and M. -Y. Wu, 'Performance-effective and low-complexity task scheduling for directed acyclic graphs,' IEEE Transactions on Parallel and Distributed Systems, vol. 13, no. 3, pp. 260-274, March 2002.")
    ]

    for idx, ref_text in enumerate(refs):
        y = Inches(1.6 + idx * 0.76)
        card = s10.shapes.add_shape(MSO_SHAPE.ROUNDED_RECTANGLE, Inches(0.8), y, Inches(11.733), Inches(0.68))
        card.fill.solid()
        card.fill.fore_color.rgb = C_CARD
        card.line.color.rgb = C_BORDER
        card.line.width = Pt(1)

        tbox = card.text_frame
        tbox.word_wrap = True
        p = tbox.paragraphs[0]
        p.text = ref_text
        p.font.name = "Arial"
        p.font.size = Pt(10)
        if "BASE PAPER" in ref_text:
            p.font.bold = True
            p.font.color.rgb = C_ACCENT
            card.line.color.rgb = C_ACCENT
            card.line.width = Pt(1.5)
        else:
            p.font.color.rgb = C_SECONDARY

    # ==========================================
    # SLIDE 11: THANK YOU SLIDE
    # ==========================================
    s11 = prs.slides.add_slide(blank_layout)
    bg11 = s11.shapes.add_shape(MSO_SHAPE.RECTANGLE, 0, 0, Inches(13.333), Inches(7.5))
    bg11.fill.solid()
    bg11.fill.fore_color.rgb = C_PRIMARY
    bg11.line.fill.background()

    t_box11 = s11.shapes.add_textbox(Inches(1.5), Inches(2.0), Inches(10.333), Inches(3.5))
    tf11 = t_box11.text_frame
    tf11.word_wrap = True

    p11_1 = tf11.paragraphs[0]
    p11_1.text = "Thank You!"
    p11_1.font.size = Pt(48)
    p11_1.font.bold = True
    p11_1.font.color.rgb = RGBColor(255, 255, 255)
    p11_1.alignment = PP_ALIGN.CENTER

    p11_2 = tf11.add_paragraph()
    p11_2.text = "Questions & Discussion"
    p11_2.font.size = Pt(22)
    p11_2.font.bold = True
    p11_2.font.color.rgb = RGBColor(147, 197, 253)
    p11_2.alignment = PP_ALIGN.CENTER

    p11_3 = tf11.add_paragraph()
    p11_3.text = "\nProject SchedulX • Pure C Engine • Full DSA Implementation"
    p11_3.font.size = Pt(14)
    p11_3.font.color.rgb = RGBColor(203, 213, 225)
    p11_3.alignment = PP_ALIGN.CENTER

    p11_4 = tf11.add_paragraph()
    p11_4.text = "GitHub: github.com/Apvikas1267/SchedulX"
    p11_4.font.size = Pt(13)
    p11_4.font.color.rgb = RGBColor(96, 165, 250)
    p11_4.alignment = PP_ALIGN.CENTER

    output_path = os.path.abspath("SchedulX_Presentation.pptx")
    prs.save(output_path)
    print(f"Presentation generated successfully: {output_path}")

if __name__ == "__main__":
    create_deck()
