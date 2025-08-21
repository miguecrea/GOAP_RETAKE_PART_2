# Gameplay Programming: Goal- Oriented Action Planning

# Explanation

* **Goal-Oriented Action Planning (GOAP)** is an AI decision-making architecture.  
* Its goals are to create intelligent, flexible, and believable agent behavior.  
* GOAP enables agents (NPCs or bots) to dynamically select and sequence actions to achieve specific goals based on the current world state.

### **Key Concepts of GOAP:**

1. **World State**  
    A representation of the current state of the game world. It’s usually a set of key-value pairs or predicates (e.g., `hasWeapon = true`, `enemyVisible = false`).

2. **Goals**  
    Desirable world states that the agent wants to achieve (e.g., `enemyDead = true`, `isHealthy = true`). Agents can have multiple goals with priorities.

3. **Actions**  
    Modular units that change the world state. Each action defines:

   * **Preconditions**: What must be true to execute it (e.g., `hasWeapon = true`)

   * **Effects**: What becomes true after executing it (e.g., `enemyDead = true`)

   * **Cost**: Numerical value representing how "expensive" or difficult the action is, influencing plan selection

4. **Planner**  
    A search algorithm (usually A\*, BFS, or DFS) that finds a valid sequence of actions (a **plan**) from the current world state to the goal. The best plan is typically the least costly valid path.

### **How It Works (Simplified Version):**

1. The AI evaluates the current world state.

2. It selects a goal to pursue based on priorities or urgency.

3. The planner searches for a sequence of actions that:

   * Meet the goal’s conditions

   * Are executable from the current world state

4. It executes the first action in the plan and re-evaluates the plan as the world state changes.

### **Advantages:**

* **Dynamic**: Plans are created at runtime, adapting to the changing world.

* **Scalable**: Easier to extend with new actions and goals without reworking the entire system.

* **More natural behavior**: AI appears more intelligent and less scripted.

### **Challenges:**

* **Computational cost**: Planning can be slow if not optimized (especially in real-time games).

* **Complexity**: More difficult to debug and balance than FSMs or behavior trees.

* **Requires solid world modeling**: The quality of decisions depends on the accuracy of world state and effects.

# Personal Implementation

## World States:

![World States](images/WorldStates.png)

![IsZombieInView World State](images/IsZombieInView.png)
IsZombieInView World State

## Actions:

![Actions](images/Actions.png)

![MoveIntoHouse Action: Searches House thoroughly](images/SearchHouse.png)
MoveIntoHouse Action: Searches House thoroughly

![Wander Action](images/Wander.png)
Wander Action: The pillar of agent new discovery

## Goals In Order Of Importance:

![Goals](images/Goals.png)

## Steps

### Overall Steps

1. Update World State  
2. Update Memory  
   1. Keep track of entity location  
      1. Items  
      2. PurgeZones  
         1. Forget after certain time  
      3. Houses  
         1. Keep track if you visited or not  
   2. Functions to interact with said data  
3. Select action plan based on world state, actions and goals  
   1. For each goal  
      1. If goal state \!= world state  
         1. Make graph of current goal  
         2. Find Optimal path using Dijkstra  
         3. If path with goal found  
            1. chosen action \= first action in plan  
         4. Else repeat with next goal  
   2. If all goals are fulfilled  
      1. chosen action \= default action  
         1. Wander  
   3. currentAction.ExecuteAction

### Making The Graph

#### Steps

1. Create two conceptual nodes  
   1. start node (signaling the start of the action plan path)  
   2. end node (signaling the end of the action plan path)  
2. Start by connecting all nodes whose conditions are met to the start node  
3. Then connect all nodes whose effect on the world is achieving the goal  
4. Interconnect All actions by checking if their effect on the world matches with another action’s preconditions.  
   1. From (action that affects world) \-\> to (action that needs said precondition)  
   2. The cost of the action that affects the world is the cost of the link

#### Code

![MakeGraph code](images/MakeGraph.png)

#### Visualization

![DijkstraVisualization](images/DijkstraVisualization.png)

### Dijkstra

#### Inputs

- Graph  
- Start Node  
- Goal Node

#### Steps

1. Things to consider:  
   1. Start searching in the starting node  
   2. You have an container that keeps track of the visited nodes and their optimal from- to neighbours for the path  
   3. You have a container that keeps track of the nodes to visit  
2. While you have more nodes to search, or while you haven’t reached the goal node  
   1. currentNode \== node with the least cost form the nodes to visit  
   2. For each neighbour node of the current node  
      1. Calculate new cost \-\> newCost \= currentNode.CostSoFar \+ connection.Cost  
      2. if you haven’t visited the node, or if the newCost is lower than the currentCostSoFar of the neighbour node  
         1. add or update their optimal from-to neighbours for the path (including the newCost) in the visited nodes  
         2. Add the neighbour node to nodes to visit  
3. Once the previous loop ends you fetch the data from the visited nodes and their optimal from-to neighbours for the path

![Dijkstra code](images/Dijkstra.png)
