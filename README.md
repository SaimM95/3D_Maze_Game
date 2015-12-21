# 3D_Maze_Game

A 3D maze made using OPENGL

Description:
- Basic 2d maze extruded to 3d environment
scraped- Have bird's eye view (that can be toggled on/off) of part of the maze based on the player's position - top right of the screen
- Have movable boxes that will aid in holding down push buttons in the environment
    - Push buttons raise a wall up while pressed
    - Have player mesh (simple boxes dragging on the floor; no animation needed)
    - Have camera working for first/third person views
    - Have enemies (simple box meshes) that terminate the game if touched by the player mesh

Slack integration test

Controls:
	- Moving the position of the player is done with the W,A,S,D keys
		- W moves forward
		- A move left
		- S moves back
		- D moves right
	- The camera is controlled by moving the mouse to the left of right of the screen (no axis inversion).

Features:
	- A minimap is displayed with a model of the current level, and diplays the position of the player.
	- Enemies will roam around the map.  To dispose enemies, you can click on them using the right mouse button.
	- The design of the maze is randomly generated.