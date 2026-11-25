import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as patches

def draw_pipeline():
    fig, ax = plt.subplots(figsize=(11, 6.5), dpi=300)
    ax.set_xlim(0, 11)
    ax.set_ylim(0, 7.5)
    ax.axis('off')

    # Styles
    box_face = '#f1f3f5'
    box_edge = '#212529'
    lw = 1.4

    # ---------------------------------------------
    # LEFT PIPELINE
    # ---------------------------------------------
    # 1. Input Box
    b1 = patches.FancyBboxPatch((0.5, 6.3), 6.5, 0.8, boxstyle="round,pad=0.08,rounding_size=0.12",
                                facecolor=box_face, edgecolor=box_edge, linewidth=lw)
    ax.add_patch(b1)
    ax.text(3.75, 6.8, "Input Dataset", ha='center', va='center', fontsize=11, fontweight='bold', color='#111')
    ax.text(3.75, 6.5, "PIDs, arrival times, burst times, priority & dependencies", ha='center', va='center', fontsize=8.5, color='#444')

    # Arrow 1 -> 2
    ax.annotate('', xy=(3.75, 5.5), xytext=(3.75, 6.3),
                arrowprops=dict(arrowstyle="->", lw=1.5, color='#111'))

    # 2. Process Store & Hash Index
    b2 = patches.FancyBboxPatch((0.5, 4.7), 6.5, 0.8, boxstyle="round,pad=0.08,rounding_size=0.12",
                                facecolor=box_face, edgecolor=box_edge, linewidth=lw)
    ax.add_patch(b2)
    ax.text(3.75, 5.2, "PROCESS STORE & HASH INDEX", ha='center', va='center', fontsize=11, fontweight='bold', color='#111')
    ax.text(3.75, 4.9, "singly linked list  •  djb2 hash table separate chaining", ha='center', va='center', fontsize=8.5, color='#444')

    # Arrow 2 -> 3
    ax.annotate('', xy=(3.75, 3.9), xytext=(3.75, 4.7),
                arrowprops=dict(arrowstyle="->", lw=1.5, color='#111'))

    # 3. Precedence Analyzer (DAG)
    b3 = patches.FancyBboxPatch((0.5, 3.1), 6.5, 0.8, boxstyle="round,pad=0.08,rounding_size=0.12",
                                facecolor=box_face, edgecolor=box_edge, linewidth=lw)
    ax.add_patch(b3)
    ax.text(3.75, 3.6, "PRECEDENCE ANALYZER (DAG)", ha='center', va='center', fontsize=11, fontweight='bold', color='#111')
    ax.text(3.75, 3.3, "Kahn's topological sort  •  DFS 3-color cycle detection", ha='center', va='center', fontsize=8.5, color='#444')

    # Branches to 3 Algorithm dispatch boxes
    ax.annotate('', xy=(1.55, 2.3), xytext=(3.75, 3.1),
                arrowprops=dict(arrowstyle="->", lw=1.2, color='#111'))
    ax.annotate('', xy=(3.75, 2.3), xytext=(3.75, 3.1),
                arrowprops=dict(arrowstyle="->", lw=1.2, color='#111'))
    ax.annotate('', xy=(5.95, 2.3), xytext=(3.75, 3.1),
                arrowprops=dict(arrowstyle="->", lw=1.2, color='#111'))

    # Algorithm 1: FCFS
    b_fcfs = patches.FancyBboxPatch((0.5, 1.45), 2.1, 0.85, boxstyle="round,pad=0.08,rounding_size=0.1",
                                    facecolor='white', edgecolor=box_edge, linewidth=lw)
    ax.add_patch(b_fcfs)
    ax.text(1.55, 1.95, "FCFS", ha='center', va='center', fontsize=10.5, fontweight='bold', color='#111')
    ax.text(1.55, 1.65, "FIFO linked queue", ha='center', va='center', fontsize=8, color='#444')

    # Algorithm 2: SJF / SRTF / PRIORITY
    b_heap = patches.FancyBboxPatch((2.7, 1.45), 2.1, 0.85, boxstyle="round,pad=0.08,rounding_size=0.1",
                                    facecolor='white', edgecolor=box_edge, linewidth=lw)
    ax.add_patch(b_heap)
    ax.text(3.75, 2.0, "SJF / SRTF / PRIO", ha='center', va='center', fontsize=9.5, fontweight='bold', color='#111')
    ax.text(3.75, 1.65, "binary min-heap", ha='center', va='center', fontsize=8, color='#444')

    # Algorithm 3: Round Robin
    b_rr = patches.FancyBboxPatch((4.9, 1.45), 2.1, 0.85, boxstyle="round,pad=0.08,rounding_size=0.1",
                                  facecolor='white', edgecolor=box_edge, linewidth=lw)
    ax.add_patch(b_rr)
    ax.text(5.95, 1.95, "ROUND ROBIN", ha='center', va='center', fontsize=9.5, fontweight='bold', color='#111')
    ax.text(5.95, 1.65, "circular array queue", ha='center', va='center', fontsize=8, color='#444')

    # Arrows to Execution & Gantt Generator
    ax.annotate('', xy=(3.75, 0.85), xytext=(1.55, 1.45),
                arrowprops=dict(arrowstyle="->", lw=1.2, color='#111'))
    ax.annotate('', xy=(3.75, 0.85), xytext=(3.75, 1.45),
                arrowprops=dict(arrowstyle="->", lw=1.2, color='#111'))
    ax.annotate('', xy=(3.75, 0.85), xytext=(5.95, 1.45),
                arrowprops=dict(arrowstyle="->", lw=1.2, color='#111'))

    # Final Output box
    b_out = patches.FancyBboxPatch((0.5, 0.05), 6.5, 0.8, boxstyle="round,pad=0.08,rounding_size=0.12",
                                   facecolor=box_face, edgecolor=box_edge, linewidth=lw)
    ax.add_patch(b_out)
    ax.text(3.75, 0.55, "EXECUTION ENGINE & GANTT TIMELINE", ha='center', va='center', fontsize=10.5, fontweight='bold', color='#111')
    ax.text(3.75, 0.25, "preemption  •  history stack trace  •  performance metrics (AWT, ATT, Util %)", ha='center', va='center', fontsize=8, color='#444')

    # ---------------------------------------------
    # RIGHT PANE: DATA STRUCTURES
    # ---------------------------------------------
    # Outer container
    outer = patches.FancyBboxPatch((7.6, 0.05), 2.9, 7.05, boxstyle="round,pad=0.12,rounding_size=0.15",
                                   facecolor='white', edgecolor=box_edge, linewidth=1.5)
    ax.add_patch(outer)
    ax.text(9.05, 6.85, "Data structures", ha='center', va='center', fontsize=12, fontweight='bold', color='#111')
    ax.text(9.05, 6.6, "updated for every CPU dispatch", ha='center', va='center', fontsize=8, color='#555')

    ds_items = [
        ("Hash table", "djb2 separate chaining\nO(1) PID lookup", 5.65),
        ("Linked list", "process store & traversal\nO(1) tail insertion", 4.6),
        ("Binary min-heap", "sift-up & sift-down\nO(log n) extract-min", 3.55),
        ("Circular queue", "modulo wrap-around\nRound Robin quantum", 2.5),
        ("Stack", "LIFO execution trace\nhistory playback", 1.45),
        ("DAG Graph", "adjacency list\ntopological sorting", 0.4)
    ]

    for title, desc, y_pos in ds_items:
        d_box = patches.FancyBboxPatch((7.85, y_pos), 2.4, 0.85, boxstyle="round,pad=0.06,rounding_size=0.1",
                                       facecolor=box_face, edgecolor=box_edge, linewidth=1.1)
        ax.add_patch(d_box)
        ax.text(9.05, y_pos + 0.55, title, ha='center', va='center', fontsize=10, fontweight='bold', color='#111')
        ax.text(9.05, y_pos + 0.25, desc, ha='center', va='center', fontsize=7.5, color='#444')

    plt.tight_layout()
    os.makedirs('docs', exist_ok=True)
    out_img = 'docs/figure_2_1_pipeline.png'
    plt.savefig(out_img, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"Diagram created: {out_img}")

if __name__ == "__main__":
    draw_pipeline()
