
/*
 * StoryMode implements a story about a journey to the starry night.
 *
 */

#include "Mode.hpp"

struct StoryMode : Mode {
	StoryMode();
	virtual ~StoryMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//called to create menu for current scene:
	void enter_scene();

	//------ story state -------
	enum {
		Room,
		Self,
		Street,
		Cafe,
		Crow,
		Starry
	} location = Room;
	bool room_first_visit = true;
	bool is_dream = false;
	bool seen_mirror = false;
	bool ate_bread  = false;
	bool drank_absinthe = false;
	
	glm::vec2 view_min = glm::vec2(0,0);
	glm::vec2 view_max = glm::vec2(620, 480);
};
