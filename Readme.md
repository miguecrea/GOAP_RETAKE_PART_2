
# 🎯 Goal-Oriented Action Planning (GOAP) System

This project implements a **GOAP (Goal-Oriented Action Planning)** framework for intelligent agents in games.  
GOAP allows AI characters to dynamically **plan sequences of actions** to achieve goals, instead of following hardcoded scripts.  

---

## 🚀 What is GOAP?

GOAP is an AI technique where agents decide *what to do* by planning a sequence of actions that lead from the **current world state** to a **desired goal state**.  

Instead of scripting behaviors manually, you define:  
- **World States** (facts about the world)  
- **Actions** (things the agent can do, with preconditions & effects)  
- **Goals** (desired states)  

The planner automatically finds the best sequence of actions (a plan) to reach a goal.  

---

## 🧩 System Overview

### Core Components
- **`BaseWorldState`**  
  Represents facts about the world.  
  Example: `HasWeapon = true`, `EnemyDead = false`.

- **`BaseAction`**  
  Defines an action the agent can perform.  
  - **Preconditions** → must be true before the action can run.  
  - **Effects** → world changes after action succeeds.  
  - **Cost/Weight** → numeric cost for planning.  

- **`Planner`**  
  The brain of the system:  
  - Builds a graph of actions (`CreateGraph`)  
  - Finds valid plans using **Dijkstra’s algorithm**  
  - Selects the best action sequence to reach a goal (`ChooseCurrentAction`)  
  - Executes actions step by step (`CalculateAction`)  

---

## 🔄 Planning Flow

1. **Graph Construction**  
   - Each action is a **node**.  
   - If Action A’s effect matches Action B’s precondition → add a connection.  

2. **Pathfinding**  
   - Uses **Dijkstra** to precompute shortest paths between all actions.  

3. **Plan Selection**  
   - For each goal, the planner finds the lowest-cost action sequence.  

4. **Execution**  
   - Current action is executed.  
   - If it succeeds → update world state and advance to next action.  
   - If it fails → recompute a new plan.  

---

## ✅ Example

**Goal:** `EnemyDead = true`  

**Actions Defined:**  
- `FindWeapon` (no weapon → has weapon)  
- `PickUpWeapon` (weapon available → has weapon)  
- `AttackEnemy` (has weapon → enemy dead)  

**Generated Plan:**  
