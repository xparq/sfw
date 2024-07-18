#include "sfw/GUI.hpp"

#include "SAL/util/diagnostics.hpp"

#include <SFML/Graphics.hpp>

#include <string> // to_string
#include <iostream> // cerr, for errors, cout for some "demo" info
#include <thread>
#include <chrono>

using namespace std;


void background_thread(sfw::GUI& gui);
static auto toy_anim_on = false;


int main()
try {
	//------------------------------------------------------------------------
	// Normal SFML app-specific setup....

	// Create the main window, as usual for any SFML app
	sf::RenderWindow window;
	window.create(sf::VideoMode({1024, 768}), "SFW Demo", sf::Style::Close|sf::Style::Resize);
	if (!window.isOpen()) {
		cerr << "- Failed to create the SFML window!\n";
		return EXIT_FAILURE;
	}
	window.setFramerateLimit(30);

	//------------------------------------------------------------------------
	// Setting up the GUI...

	// Just to spare some noise & typing...
	using sfw::Theme, sfw::Color, sfw::VBox, sfw::HBox, sfw::Form;
	using OBColor = sfw::OptionsBox<Color>;

	// Customizing some theme props. (optional)

//	Theme::DEFAULT.basePath = "demo/";
//	Theme::DEFAULT.fontFile = "font/Vera.ttf"; // relative to basePath

	Theme::PADDING = 2.f;
	Theme::click.textColor      = Color("#191B18");
	Theme::click.textColorHover = Color("#191B18");
	Theme::click.textColorFocus = Color("#000");
	Theme::input.textColor      = Color("#000");
	Theme::input.textColorHover = Color("#000");
	Theme::input.textColorFocus = Color("#000");
	Theme::input.textColorDisabled = Color("#888");
	Theme::input.textSelectionColor = Color("#b0c0c0");
	Theme::input.textPlaceholderColor = Color("#8791AD");

	// Prepare some dynamically switchable theme config "quick packs" to play with...

	// Raw C++ init lists (with rigid positions):
	Theme::Cfg themes[] = {
		{ "Baseline", "demo/", "texture-sfw-baseline.png", Color("#e6e8e0"),
		  sfw::Wallpaper::Cfg{"demo/wallpaper.jpg", sfw::Wallpaper::Center, Color(255,255,255,63)},
		  17, "font/LiberationSans-Regular.ttf" },
		{ "Classic ☺",              "demo/", "texture-sfw-classic.png",  Color("#e6e8e0"), {}, 12, "font/LiberationSans-Regular.ttf" },
		{ "sfml-widgets's default", "demo/", "texture-sfmlwidgets-default.png", Color("#dddbde"), {}, 12, "font/Vera.ttf" },
		{ "sfml-widgets's Win98",   "demo/", "texture-sfmlwidgets-win98.png",   Color("#d4d0c8"), {}, 12, "font/Vera.ttf", /*.multiTooltips =*/ true },
		{ "factory default" },
	};
/*	// More flexible (but also more cumbersome) with std::map:
	Theme::Cfg themes[] = {
		{ {"name", "Baseline"}, {"base_path", "demo/"}, {"textures", "texture-sfw-baseline.png"},
		       {"bg_color", {Color("#e6e8e0")}, {"wallpaper", {"asset/wallpaper.jpg", sfw::Wallpaper::Center, #ffffff3f}},
		       {"default_font", {"font/Liberation/LiberationSans-Regular.ttf", 17} },
		{ {"name", "Classic ☺"},              {"base_path", "demo/"}, {"textures", "texture-sfw-classic.png"}, {"bg_color", Color("#e6e8e0")}, {"default_font", {"font/Liberation/LiberationSans-Regular.ttf", 12}} },
		{ {"name", "sfml-widgets's default"}, {"base_path", "demo/"}, {"textures", "texture-sfmlwidgets-default.png"}, {"bg_color", Color("#dddbde")}, {"default_font", 12} },
		{ {"name", "sfml-widgets's Win98"},   {"base_path", "demo/"}, {"textures", "texture-sfmlwidgets-win98.png"}, {"bg_color", Color("#d4d0c8")},   {"default_font", 12} },
		{ {"name", "factory default"} },
	};
*//*	// Ultimately, parsed:
	Theme::Cfg themes[] = {
		{ R"_( name = "Baseline", base_path = "demo/", textures = "texture-sfw-baseline.png", bg_color = #e6e8e0,
		       wallpaper = {"asset/wallpaper.jpg", sfw::Wallpaper::Center, #ffffff3f},
		       default_font = {"font/Liberation/LiberationSans-Regular.ttf", 17} )_",
		{ R"_( name = "Classic ☺",              base_path = "demo/", textures = "texture-sfw-classic.png", bg_color = #e6e8e0, default_font = {"font/Liberation/LiberationSans-Regular.ttf", 12} )_",
		{ R"_( name = "sfml-widgets's default", base_path = "demo/", textures = "texture-sfmlwidgets-default.png", bg_color = #dddbde, default_font = {"font/Vera.ttf", 12} )_",
		{ R"_( name = "sfml-widgets's Win98",   base_path = "demo/", textures = "texture-sfmlwidgets-win98.png", bg_color = #d4d0c8,   default_font = {"font/Vera.ttf", 12} )_",
		{ R"_( name = "factory default" )_",
	};
*/
	const size_t DEFAULT_THEME = 0;

	//--------------------------------------------------------------------
	// Create the main GUI controller:
	sfw::GUI demo(window, themes[DEFAULT_THEME]);
	if (!demo) {
		return EXIT_FAILURE; // Errors have already been printed to cerr.
	}
	demo.setPosition(10, 10);

	//--------------------------------------------------------------------
	// Some raw, native SFML example shapes (sf::Text + a filled rect),
	// to be manipulated by other widgets...
	// + a wrapper widget for boxing them nicely

		sfw::gfx::Text text("Hello world!");
		sf::RectangleShape textrect;

	auto sfText = new sfw::DrawHost([&](auto* widget, auto ctx) { // This entire function is a draw() hook!
		using namespace sfw; using namespace sfw::geometry;

		//! Note: other widgets will update the sample text & its style!

		// Set a rect. large enough to contain the entire transformed (rotated, scaled etc.) text.
		// That will also be the (variable!) size of the widget.
		//! (Note: sf::Text's bound-rect would not be 0-based, throwing off
		//! any unsuspecting geometry operations! (-> SFML #216)
		//! But with sfw::gfx::Text we don't need to deal with that here.)
		fRect bgrect({}, {text.size().x() * 1.2f, text.size().y() * 3.5f});
		fVec2 containerSize = text.getTransform().transformRect(bgrect).size; //!! Confusing mix of SFW and SFML APIs! :-/
//!!??		auto actualBoundRect = xform.transformRect(textrect.getGlobalBounds());
//!!?? Why's getGlobalBounds different here?!
//!!?? It does make the widget resize correctly -- which is the only place this
//!!?? rect is used --, but then the shape itself will stay the same size, and the
//!!?? rotation would be totally off! :-o It's all just queries! WTF?! :-ooo

		//!!--------------------------------------------------------------------------------------
		//!! Umm... *GROSS*! Triggers a full GUI layout reflow, right from a draw() call!... :-o
		widget->setSize(containerSize + 2 * Theme::PADDING);
		//!!--------------------------------------------------------------------------------------

		//!! Must position the shapes after widget->setSize, as that may move the widget
		//!! out from under the content we manually hack here, leaving the appearance off
		//!! for the current frame! :-o (A very visible artifact, as I've learned!...)
		//!! Well, another disadvantage of calling setSize during draw()!...
		//!! Fortunately, this one can be tackled by doing this after setSize:

		// Sync the bg. rect to various "externalia":
		textrect.setScale(text.getScale());
		textrect.setRotation(text.getRotation());
		textrect.setSize(bgrect.size());
		textrect.setOrigin({textrect.getSize().x / 2, textrect.getSize().y / 2});
		textrect.setPosition(widget->getSize() / 2);

		text.center_in({{}, widget->getSize()}); //! Also changes its origin (to the center of the bounding box)!

		// Draw, finally:
		auto sfml_renderstates = ctx.props;
		sfml_renderstates.transform *= widget->getTransform();
		ctx.target.draw(textrect, sfml_renderstates);
		text.draw(gfx::RenderContext{ctx.target, sfml_renderstates});
#ifdef DEBUG
		widget->draw_outline(ctx);
#endif
	});


	//--------------------------------------------------------------------
	// A horizontal layout for multiple panels side-by-side
	auto main_hbox = demo.add(new HBox);


	//--------------------------------------------------------------------
	// A "form" panel on the left
	auto left_panel = main_hbox->add(new VBox);

	auto form = left_panel->add(new Form);

	// A text box to set the text of the example SFML object (created above)
	form->add("Text", new sfw::TextBox(175)) // <- width (pixels)
		->setPlaceholder("Type something!")
		->set("Hello world!")
		->setCallback([&](auto* w) { text.set(w->get()); });
	// Another text box, with length limit & pulsating cursor
	form->add("Text with limit (5)", new sfw::TextBox(50.f, sfw::TextBox::CursorStyle::PULSE))
		->setMaxLength(5)
		->set("Hello world!");

	// Slider + progress bar for scaling
	auto sliderForSize = new sfw::Slider({.length = 175, .range = {1, 3}, .step = .2f});
	auto pbarScale = new sfw::ProgressBar({.length = 175});
	sliderForSize->setCallback([&](auto* w) {
		float scale = w->get();
		text.setScale({scale, scale}); // (x, y)
		pbarScale->set((w->get() - 1) / 2.f * 100);
	});
	// Slider + progress bar for rotating
	auto sliderForRotation = new sfw::Slider({.length=75, .range={0, 360}, .orientation=sfw::Vertical});
	auto pbarRotation = new sfw::ProgressBar({.length=75, .range={0, 360}, .orientation=sfw::Vertical, .unit="°"});
	sliderForRotation->setCallback([&](auto* w) {
		text.setRotation(sf::degrees(w->get()));
		pbarRotation->set(w->get());
	});

	// Add the scaling slider + its horizontal progress bar
	form->add("Size", sliderForSize);
	form->add("", pbarScale);

	// Add the rotation slider + its vertical progress bar
        // ...And, since there would be too much wasted space next to the p.bar,
	// put some text-styling checkboxes there. :)
	// (Note the use of the "new-less" (move-constructing) add(...) style, for a change!)
	auto rot_and_chkboxes = form->add("Rotation", HBox());
		rot_and_chkboxes->add(sliderForRotation, "rotation-slider");
		rot_and_chkboxes->add(pbarRotation);
		rot_and_chkboxes->add(sfw::Label("         ")); // Just a (vert.) spacer...

		auto styleboxes = rot_and_chkboxes->add(Form());
		// Checkboxes to set text properties (using all 3 query styles):
		styleboxes->add("Bold text", sfw::CheckBox([&](auto* w) {
			int style = text.getStyle();
			if (*w) style |=  sf::Text::Bold;
			else    style &= ~sf::Text::Bold;
			text.setStyle(style);
		}));
		styleboxes->add("Italic", sfw::CheckBox([&](auto* w) {
			int style = text.getStyle();
			if (w->checked()) style |=  sf::Text::Italic;
			else              style &= ~sf::Text::Italic;
			text.setStyle(style);
		}));
		styleboxes->add("Underlined", sfw::CheckBox([&](auto* w) {
			int style = text.getStyle();
			if (w->isChecked()) style |=  sf::Text::Underlined;
			else                style &= ~sf::Text::Underlined;
			text.setStyle(style);
		}));

	// Color selectors
	// -- Creating one as a template to clone it later, because
	//    WIDGETS MUSTN'T BE COPIED AFTER HAVING BEEN ADDED TO THE GUI!
	OBColor ColorSelect_TEMPLATE; (&ColorSelect_TEMPLATE)
		->add("Black" , Color::Black)
		->add("Red"   , Color::Red)
		->add("Green" , Color::Green)
		->add("Blue"  , Color::Blue)
		->add("Cyan"  , Color::Cyan)
		->add("Yellow", Color::Yellow)
		->add("White" , Color::White)
	;

	// Select sample text color
	auto optTxtColor = (new OBColor(ColorSelect_TEMPLATE))
		->setCallback([&](auto* w) {
			text.setFillColor(w->current());
			w->setTextColor(w->current());
		});

	// Select sample text box color
	auto optTxtBg = (new OBColor(ColorSelect_TEMPLATE))
		->add("Default", themes[DEFAULT_THEME].bgColor)
		->setCallback([&](auto* w) {
			textrect.setFillColor(w->current());
			w->setFillColor(w->current() * Color(255, 255, 255, 64));
		});

	// (See also another one for window bg. color selection!)

	form->add("Text color", optTxtColor);
	form->add("Box color", optTxtBg);


	//--------------------------------------------------------------------
	// Buttons...
	auto buttons_vbox = form->add("Buttons", sfw::VBox());

	// "Button factory" labeller textbox + creat button...
	auto buttfactory = buttons_vbox->add(new HBox);
	auto labeller = buttfactory->add(sfw::TextBox(100))->set("Edit Me!")->setPlaceholder("Button label")
		->setCallback([&] {
			demo.call<sfw::Button>("button spawner", [](auto* b){ b->click(); });
// OR (LESS DENSE):	if (auto b = demo.find_widget<sfw::Button>("button spawner"); b) b->click();
		});
	buttfactory->add(sfw::Button("Create button", [&] {
		buttons_vbox->addAfter(buttfactory, new sfw::Button(labeller->get()));
	}), "button spawner");

	// More buttons...
	auto buttons_form = buttons_vbox->add(sfw::Form());

	auto utf8button_tag = "UTF-8 by default:"; // We'll also use this text to find the button.
	buttons_form->add(utf8button_tag, sfw::Button("Ψ ≠ 99° ± β")); // Note: this source is already UTF-8 encoded!
	cout << "UTF-8 button text got back as: \"" << demo.find_widget<sfw::Button>(utf8button_tag)->getText() << "\"\n";

	// Bitmap buttons
	auto imgbuttons_form = left_panel->add(sfw::Form());
	sfw::gfx::Texture buttonimg; //! DON'T put this texture inside the if() as a local temporary!... ;)
	if (buttonimg.load("demo/sfmlwidgets-themed-button.png")) // SFML would print an error if failed
	{
		auto combined_labels = new VBox();
		combined_labels->add(sfw::Label("Image button"));
		combined_labels->add(sfw::Label("- native size"));

		imgbuttons_form->add(combined_labels, new sfw::ImageButton(buttonimg, "Start rotating"))
			->setFontSize(20)
			->setCallback([]/*(auto* w)*/ { toy_anim_on = true; });

		imgbuttons_form->add("- customized", new sfw::ImageButton(buttonimg, "Stop"))
			->setFontSize(20)
			->setTextStyle(sf::Text::Style::Bold)
			->setSize({180, 35})
			->setTextColor(Color("#d0e0c0"))
			->setCallback([]/*(auto* w)*/ { toy_anim_on = false; });
	}


	//--------------------------------------------------------------------
	// A panel in the middle
	auto middle_panel = main_hbox->add(new VBox);

	// OK, add the SFML test shapes + adapter widget here
	middle_panel->add(sfText);


	//--------------------------------------------------------------------
	// Image views...

	// Image directly from file
	auto vboximg = main_hbox->add(new VBox);
	vboximg->add(sfw::Label("Image (directly from file):"));
	vboximg->add(sfw::Image("demo/example.jpg"));

	// Image crop + scaling directly from file
	vboximg->add(sfw::Label("Image crop + zoom:"));
	vboximg->add(sfw::Image("demo/example.jpg", {{100, 110}, {30, 20}}))->scale(3);

	// Image from file, cropped dynamically
	vboximg->add(sfw::Label("Dynamic cropping:"));

	sfw::Image* imgCrop = new sfw::Image("demo/martinet-dragonfly.jpg");
	// Slider & progress bar for cropping an Image widget
	// (`jumpy_thumb_click = true` allows immediately readjusting the pos.
	// of the slider thumb on clicking it)
	auto cropslider = (new sfw::Slider({.length = 100, .jumpy_thumb_click = true}))->setCallback([&](auto* w) {
		demo.set<sfw::ProgressBar>("cropbar", w->get());
		// Show the slider value in a text box retrieved by its name:
		auto tbox = demo.find_widget<sfw::TextBox>("crop%");
		if (!tbox) cerr << "Named TextBox not found! :-o\n";
		else tbox->set(w->get() ? to_string((int)w->get()) : "");
		imgCrop->setCropRect({{(int)(w->get() / 4), (int)(w->get() / 10)},
		                      {(int)(w->get() * 1.4), (int)w->get()}});
	});

	vboximg->add(cropslider);
	auto boxcrop = vboximg->add(new HBox);
		boxcrop->add(sfw::Label("Crop square size:"));
		boxcrop->add(sfw::ProgressBar(47), "cropbar");
		boxcrop->add((new sfw::TextBox(36))->setMaxLength(3), "crop%")->setCallback([&](auto* w) {
			try { cropslider->update(stof(w->get())); }
			catch (...) { cropslider->set(0); }
		});

	vboximg->add(imgCrop);
	vboximg->add(sfw::Label("(Art: © Édouard Martinet)"))->setStyle(sf::Text::Style::Italic);

	//--------------------------------------------------------------------
	// Another "sidebar" column, for (theme) introspection...
	auto right_bar = main_hbox->add(new VBox);

	// Theme selection
	using OBTheme = sfw::OptionsBox<Theme::Cfg>;
	auto themeselect = new OBTheme([&](auto* w) {
		const auto& themecfg = w->current();
		if (demo.setTheme(themecfg))
		{
			// Update the wallpaper controls:
			demo.call<sfw::CheckBox>("Wallpaper", [&](auto* wptoggle) {
				wptoggle->enable(demo.hasWallpaper());
				wptoggle->set(demo.hasWallpaper()); });
			demo.call<sfw::Slider>("Wallpaper α", [&](auto* wpalpha) {
				wpalpha->enable(demo.hasWallpaper());
				if (demo.hasWallpaper()) wpalpha->update(themecfg.wallpaper.tint.a()); });
			// Update the bg. color selector's "Default" value:
			demo.call<OBColor>("Bg. color", [&](auto* bgsel) {
				bgsel->assign("Default", themecfg.bgColor); });

			//!! Update the font size slider:
			//!! (Beware of infinite recursion, as its own onUpdate callback would
			//!! trigger another theme change, if the new font size differs!...)
			//!!...
		}
	});
	for (auto& t: themes) { themeselect->add(t.name, t); }
	themeselect->set(DEFAULT_THEME);
	right_bar->add(sfw::Label("Select theme:"));
	right_bar->add(themeselect, "theme-selector");

	// Theme font size slider
	// (Also directly changes the font size of the theme cfg. data stored in the
	// "theme-selector" widget, so that it remembers the updated size (for each theme)!)
	right_bar->add(sfw::Label("Theme font size (use the m. wheel):")); // Move that remark to a tooltip!
	right_bar->add(sfw::Slider({.length = 100, .range = {8, 20}}), "font size slider")
		//!! Would be tempting to sync the initial font size directly with
		//!! the theme selector widget -- but can't: the GUI is not up yet, so...
		//!! ->set(w->getWidget<OBTheme>("theme-selector")->current().fontSize) //! getWidget would fail here!
		->set((float)themes[DEFAULT_THEME].fontSize)
		->setCallback([&] (auto* w){
			assert(demo.find_widget("theme-selector"));
// UNCHECKED:		auto& themecfg = demo.find_widget<OBTheme>("theme-selector")->current();
			demo.call<OBTheme>("theme-selector", [&](auto* ts) {
				auto& themecfg = ts->get();
				themecfg.fontSize = (size_t)w->get();
cerr << "font size: "<< themecfg.fontSize << endl; //!!#196
				demo.setTheme(themecfg);
			});
		});

	// Show the current theme texture bitmaps
	right_bar->add(sfw::Label("Theme textures:"));
	auto txbox = right_bar->add(new HBox);
	struct ThemeBitmap : public sfw::Image {
		ThemeBitmap() : Image(Theme::getTexture()) {}
		void onThemeChanged() override { setTexture(Theme::getTexture()); } // note: e.g. the ARROW is at {{0, 42}, {6, 6}}
	};
	auto themeBitmap = new ThemeBitmap; //ThemeBitmap(2); // start with 2x zoom
	txbox->add(sfw::Slider({.length = 100, .range = {0, 4}, .orientation = sfw::Vertical, .invert = true})) // height = 100
		->setCallback([&](auto* w) { themeBitmap->scale(1 + w->get()); })
		->update(3); // use update(), not set(), to trigger the callback!
	txbox->add(themeBitmap);

	right_bar->add(sfw::Label(" ")); // Just for some space, indeed...

	// Playing with the background...
	auto bgform = right_bar->add(new Form);

	// Wallpaper on/off checkbox
	bgform->add("Wallpaper", sfw::CheckBox([&](auto* w) { w->checked() ? demo.setWallpaper() : demo.disableWallpaper(); },
                                               demo.hasWallpaper())
	           )->enable(demo.hasWallpaper()); // Only enable if there actually is a wallpaper!

	// Wallpaper transparency slider
	bgform->add("Wallpaper α", sfw::Slider({.length = 65, .range={0, 255}}))
		->set(demo.getWallpaper().getColor().a())
		->setCallback([&](auto* w) {
			demo.call<OBTheme>("theme-selector", [&](auto* ts) {
				auto tint = ts->get().wallpaper.tint;
				tint.a((uint8_t)w->get());
				demo.setWallpaperColor(tint);  // Update the wallpaper itself, to make it instant...
				Theme::wallpaper.tint = tint;  // Update the current theme, to make it "robust"! (-> #419)
/*
				// And also update the current config, to make the change "persistent":
				//!! Alas, this won't work, as the fallback default is expected to be passed by-value,
				//!! and "forcibly" passing a ref will break a ?: op in Widget::get_or())...:
				//!!auto& themecfg = demo.get<OBTheme>("theme-selector", std::ref(themes[DEFAULT_THEME]));
				if (auto* ts = demo.find_widget<OBTheme>("theme-selector")) {
					auto& themecfg = ts->current();
					themecfg.wallpaper.tint.a(tint);
				}
*/
			});
		});
	// Window background color selector
	right_bar->add(new Form)->add("Bg. color", (new OBColor(ColorSelect_TEMPLATE))
		->add("Default", themes[DEFAULT_THEME].bgColor)
		->setCallback([&](auto* w) {
			Theme::bgColor = w->current();
		})
	);
	// A pretty useless, but interesting clear-background checkbox
	auto hbox5 = right_bar->add(new sfw::HBox);
	hbox5->add(sfw::Label("Clear background"));
	hbox5->add(sfw::CheckBox([&](auto* w) { Theme::clearBackground = w->checked(); }, true));

	// Custom exit button (totally redundant, but so reassuring! :) )
	demo.add(sfw::Button("Quit", [&] { demo.close(); }));

	// Set this last, otherwise the dynamic GUI resize (on adding new widgets)
	// may interfere with it!
	//demo.setWallpaper("asset/wallpaper.jpg");

	//--------------------------------------------------------------------
	// OK, GUI setup done. Update some initial widget states post-setup,
	// triggering callbacks (now that we can)! (Unlike using just set()
	// in the setup phase above.)
	//
	//!! Change the font size (to bigger?) to "prime" SFML to avoid #196! :-o
	demo.update<sfw::Slider>("font size slider", 14);

	sliderForRotation->update(104);
	sliderForSize->update(1.2f);
	// Colors of the example text + rect:
	optTxtColor->select("Red"); // Now all ready, safe to trigger the update callback (so, not just set()...)
	optTxtBg->select("Black");

	//--------------------------------------------------------------------
	// Start another thread that also manipulates some widgets:
	jthread bg_thread(background_thread, std::ref(demo));


	//--------------------------------------------------------------------
	// Event Loop
	//--------------------------------------------------------------------
//cerr<<"Starting the event loop..."<< endl;
	while (demo)
	{
		// While somewhat less elegant than the textbook double-loop solution,
		// this unrolled single-loop structure (not counting the raw-mouse-move
		// filter loop) is chosen for allowing screen updates both after input events
		// and also after some idle timeout (to support showing (polling) changes
		// made by other threads), without either awkwardly duplicating the updates
		// in a nested event-puming loop or having an initial extra draw outside
		// the main loop. (-> FizzBuzz?... ;) )
		auto event = window.pollEvent();
		while (event && event->is<sf::Event::MouseMovedRaw>()) event = window.pollEvent(); // Ignore the raw mouse-move event spam of SFML3! :-/
		if (event)
		{
			// Pass the event to the GUI:
			demo.process(event.value());

			// Close on Esc:
			if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
				demo.close();
		}

		// Redraw the UI into the rendering (frame) buffer of the connected (or some other) window:
		demo.render(/*!!maybe_to_some_other_window!!*/);
		// Show the updated window:
		window.display();

		if (!event) this_thread::sleep_for(20ms);
	}

	return EXIT_SUCCESS;

} catch(...) {
	cerr << "- ERROR: UNHANDLED EXCEPTION!" << endl;
}


//----------------------------------------------------------------------------
void background_thread(sfw::GUI& gui)
{
	auto rot_slider = gui.find_widget<sfw::Slider>("rotation-slider");
	if (!rot_slider)
		return;

	while (gui)
	{
		if (toy_anim_on)
		{
			// Cycle the rot. slider
			auto sampletext_angle = sf::degrees(rot_slider->get());
			rot_slider->update(int(sampletext_angle.asDegrees()) % 360 + 2);
		}

		this_thread::sleep_for(chrono::milliseconds(20));
		// (NOTE: the GUI may have been shut down in the meantime, and it's
		// pretty unhealthy to still keep using it then... So, this is far
		// from bullet-proof, of course, because the threads aren't synced
		// in any way! But for a demo like this, let's pretend it was OK...)

		cerr << gui.sessionTime() << "           \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
	}
}
