#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "MenuMode.hpp"

Sprite const *sprite_room = nullptr;
Sprite const *sprite_self = nullptr;
Sprite const *sprite_street = nullptr;
Sprite const *sprite_cafe= nullptr;
Sprite const *sprite_crow = nullptr;
Sprite const *sprite_starry = nullptr;

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("starry"));
	sprite_room = &ret->lookup("room");
	sprite_self = &ret->lookup("self");
	sprite_street = &ret->lookup("street");
	sprite_cafe = &ret->lookup("cafe");
	sprite_crow = &ret->lookup("crow");
	sprite_starry = &ret->lookup("starry");
	return ret;
});

StoryMode::StoryMode() {
}

StoryMode::~StoryMode() {
}

bool StoryMode::handle_event(SDL_Event const &, glm::uvec2 const &window_size) {
	if (Mode::current.get() != this) return false;

	return false;
}

void StoryMode::update(float elapsed) {
	if (Mode::current.get() == this) {
		//there is no menu displayed! Make one:
		enter_scene();
	}
}

void StoryMode::enter_scene() {
	//just entered this scene, adjust flags and build menu as appropriate:
	std::vector< MenuMode::Item > items;
	glm::vec2 at(3.0f, view_max.y - 30.0f);
	auto add_text = [&items,&at](std::string const &text) {
		items.emplace_back(text, nullptr, 1.6f, nullptr, at);
		at.y -= 30.0f;
	};
	auto add_choice = [&items,&at](std::string const &text, std::function< void(MenuMode::Item const &) > const &fn) {
		items.emplace_back(text, nullptr, 1.6f, fn, at + glm::vec2(8.0f, 0.0f));
		at.y -= 30.0f;
	};

	if (location == Room) {
		if (room_first_visit) {
			add_text("You woke up in a strange room you have never been to before...");
		} else if (!seen_mirror){
			add_text("You decide to make sure you are looking good before leaving.");
		} else {
			add_text("You are in a strange room you have never been to before...");
		}

		at.y -= 20.0f; //gap before choices

		add_choice("Look into the mirror.", [this](MenuMode::Item const &){
			location = Self;
			Mode::current = shared_from_this();
		});
		if (room_first_visit) {
			add_choice("Leave the room and go downstairs.", [this](MenuMode::Item const &){
				location = Room;
				Mode::current = shared_from_this();
			});
			room_first_visit = false;
		} 
		if (seen_mirror) {
			add_choice("Leave the room and go downstairs.", [this](MenuMode::Item const &){
				location = Street;
				Mode::current = shared_from_this();
			});
		}
	} else if (location == Self) {
		if (!seen_mirror) {
			seen_mirror = true;
		}
		if (is_dream) {
			add_text("It is indeed a dream :(");
			add_text("You took a nap in GHC and need to cotinue your work now.");
		} else {
			add_text("Looking into the mirror, you realize you are Vincent van Gogh!");
		}

		at.y -= 20.0f; //gap before choices

		if (is_dream) {
			add_choice("Bad End.", [this](MenuMode::Item const &){
				location = Self;
				Mode::current = nullptr;
			});
		} else {
			add_choice("Nice, let's explore Netherlands.", [this](MenuMode::Item const &){
				location = Street;
				Mode::current = shared_from_this();
			});
			add_choice("Impossible, it must be a dream!", [this](MenuMode::Item const &){
				is_dream = true;
				location = Self;
				Mode::current = shared_from_this();
			});
		}
	} else if (location == Street) {
		add_text("You are outside a Cafe.");

		at.y -= 20.0f; //gap before choices

		add_choice("Walk forward.", [this](MenuMode::Item const &){
			location = Crow;
			Mode::current = shared_from_this();
		});
		add_choice("Enter the Cafe.", [this](MenuMode::Item const &){
			location = Cafe;
			Mode::current = shared_from_this();
		});
		add_choice("Go back to your room.", [this](MenuMode::Item const &){
			location = Room;
			Mode::current = shared_from_this();
		});
	} else if (location == Cafe) {
		if (!ate_bread && !drank_absinthe){
			add_text("Something to eat or drink?");
		} else if (!ate_bread) {
			add_text("Something to eat?");
		} else if (!drank_absinthe) {
			add_text("Something to drink?");
		} else {
			add_text("You are too full to have anything else...");
		}
		
		at.y -= 20.0f; //gap before choices
		
		if (!ate_bread) {
			add_choice("Eat some dry bread, as you can not afford more.", [this](MenuMode::Item const &){
				ate_bread = true;
				location = Cafe;
				Mode::current = shared_from_this();
			});
		}
		if (!drank_absinthe) {
			add_choice("Drink some Van Gogh's favorite absinthe.", [this](MenuMode::Item const &){
				drank_absinthe = true;
				location = Cafe;
				Mode::current = shared_from_this();
			});
		}
		add_choice("Leave the Cafe.", [this](MenuMode::Item const &){
			location = Street;
			Mode::current = shared_from_this();
		});
	} else if (location == Crow) {
		if (!ate_bread && !drank_absinthe) {
			add_text("After a long walk, you have come to a wheatfield.");
			add_text("You are too hungry and afraid of the crows to go forward.");
		} else if (!ate_bread) {
			add_text("After a long walk, you have come to a wheatfield.");
			add_text("You are too hungry to go forward.");
		} else if (!drank_absinthe) {
			add_text("After a long walk, you have come to a wheatfield.");
			add_text("You are too afraid of the crows to go forward.");
		} else{
			add_text("You are deciding where to go.");
		}

		at.y -= 20.0f; //gap before choices
		
		if (ate_bread && drank_absinthe) {
			add_choice("Keep going forward.", [this](MenuMode::Item const &){
				location = Starry;
				Mode::current = shared_from_this();
			});
		}
		add_choice("Go back.", [this](MenuMode::Item const &){
			location = Street;
			Mode::current = shared_from_this();
		});
	} else {
		add_text("Walking through the wheatfield with crows,");
		add_text("you have come to the top of a mountain.");
		add_text("Staring at the starry night,");
		add_text("you somehow forget all the difficulties in your life.");
		add_text("You know everything is going to be alright.");

		at.y -= 20.0f;

		add_choice("The End.", [this](MenuMode::Item const &){
			location = Starry;
			Mode::current = nullptr;
		});
	}
	std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >(items);
	menu->atlas = sprites;
	menu->view_min = view_min;
	menu->view_max = view_max;
	menu->background = shared_from_this();
	Mode::current = menu;
}

void StoryMode::draw(glm::uvec2 const &drawable_size) {
	//clear the color buffer:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);
		glm::vec2 ul = glm::vec2(view_min.x, view_max.y);
		if (location == Room) {
			draw.draw(*sprite_room, ul);
		} else if (location == Self) {
			draw.draw(*sprite_self, ul);
		} else if (location == Street) {
			draw.draw(*sprite_street, ul);
		} else if (location == Cafe) {
			draw.draw(*sprite_cafe, ul);
		} else if (location == Crow) {
			draw.draw(*sprite_crow, ul);
		} else if (location == Starry) {
			draw.draw(*sprite_starry, ul);
		}
	}
	GL_ERRORS(); //did the DrawSprites do something wrong?
}
