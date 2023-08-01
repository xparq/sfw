#include "sfw/GUI.hpp"

#include <SFML/Graphics.hpp>

#include <string> // to_string
#include <iostream> // cerr, for errors, cout for some "demo" info
#include <thread>
#include <chrono>
#include <cassert>
using namespace std;

// Try/check without most of the sfw:: prefixes:
using namespace sfw;

void background_thread_main(GUI& gui);

static auto toy_anim_on = false;

int main()
{
	//------------------------------------------------------------------------
	// SFML app frame....
	sf::RenderWindow window;
	window.create(sf::VideoMode({1024, 768}), "SFW TEST: main", sf::Style::Close|sf::Style::Resize);
	if (!window.isOpen()) {
		cerr << "- Failed to create the SFML window!\n";
		return EXIT_FAILURE;
	}
	window.setFramerateLimit(30);

	//------------------------------------------------------------------------
	// Setup the Test GUI...

	using namespace sfw;
	using OBColor = sfw::OptionsBox<sf::Color>;

//	Theme::DEFAULT.basePath = "demo/";
//	Theme::DEFAULT.fontFile = "font/Vera.ttf"; // relative to basePath!

	Theme::PADDING = 2.f;
	Theme::click.textColor      = hex2color("#191B18");
	Theme::click.textColorHover = hex2color("#191B18");
	Theme::click.textColorFocus = hex2color("#000");
	Theme::input.textColor = hex2color("#000");
	Theme::input.textColorHover = hex2color("#000");
	Theme::input.textColorFocus = hex2color("#000");
	Theme::input.textColorDisabled = hex2color("#888");
	Theme::input.textSelectionColor = hex2color("#97b1AD");
	Theme::input.textPlaceholderColor = hex2color("#8791AD");

	// Some dynamically switcahble theme "quick config packs" to play with
	Theme::Cfg themes[] = {
		{ "Baseline", "demo/", "texture-sfw-baseline.png", hex2color("#e6e8e0"),
		  sfw::Wallpaper::Cfg("demo/wallpaper.jpg", sfw::Wallpaper::Center, sf::Color(255,25,25,20)),
		  11, "font/LiberationSans-Regular.ttf" },
		{ "Classic ☺",              "demo/", "texture-sfw-classic.png",  hex2color("#e6e8e0"), {}, 12, "font/LiberationSans-Regular.ttf" },
		{ "sfml-widgets's default", "demo/", "texture-sfmlwidgets-default.png", hex2color("#dddbde"), {}, 12, "font/Vera.ttf" },
		{ "sfml-widgets's Win98",   "demo/", "texture-sfmlwidgets-win98.png",   hex2color("#d4d0c8"), {}, 12, "font/Vera.ttf" },
		{ "\"factory default\"", },
	};
	const size_t DEFAULT_THEME = 0;


	//--------------------------------------------------------------------
	// Creating the main GUI controller:
	sfw::GUI demo(window, themes[DEFAULT_THEME], false);
	if (!demo) {
		return EXIT_FAILURE; // Errors have already been printed to cerr.
	}
	demo.setPosition(10, 10);

	demo.add(new Label("––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––"
	                   " MOST RECENT TEST CASES: "
	                   "––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––"));//->setStyle(sf::Text::Style::Bold);
	auto test_hbox = demo.add(new HBox);

	//--------------------------------------------------------------------
	// Part of #167: Add a generic "SFML container" widget, and put the demo text into one
	// (See the rest of #167 later below, at the other DrawHost widget!)
	auto circlevista = new DrawHost([&](auto* w, auto ctx) {
		sf::CircleShape circ(50);
		sf::Texture backdrop;
                if (!backdrop.loadFromFile("test/example.jpg")) return;
		circ.setTexture(&backdrop);
		circ.setTextureRect({{10, 10}, {100, 100}});

		auto sfml_renderstates = ctx.props;
		sfml_renderstates.transform *= w->getTransform();
		ctx.target.draw(circ, sfml_renderstates);
#ifdef DEBUG
		w->draw_outline(ctx);
#endif
	});
	circlevista->setSize(100,100)
		->setTooltip("\"circlevista\" tooltip...");

	// #168: Form supporting any left-hand-side widget as "label"
	auto labelbox = new VBox;
		labelbox->add(Label("Issue #168"));
		labelbox->add(circlevista);
	test_hbox->add(new Form)->add(labelbox, new Label("OK!"));

	// #171
	auto gh171form = test_hbox->add(new Form);
	gh171form->add("#171 autocast", new sfw::TextBox(50))->set("OK!");
	//#171 + #168 (with an Image*):
	gh171form->add((new Image("test/example.jpg"))->rescale(0.25), new sfw::TextBox(50))->set("171 + 168 OK!");
	//#171 + #168 + template widget + move
	gh171form->add(new Label("tempWidget"), sfw::TextBox(70))->set("171/tmp + 168 OK!");
	//#171, template-move also for the "label":
	gh171form->add(Label("tmpLabel"), Label("#171(tmp,tmp) OK too!"));

	//!! This is not yet supported (nor separators...):
	//!!test_hbox->add(new Form)->add("This is just some text on its own.");

	auto issuebox2 = test_hbox->add(new VBox);
	// #127 + name lookup
	issuebox2->add(Button("Issue #127/void", [&] {
		getWidget<Button>("test #127", demo)->setText("Found itself!");
	}), "test #127");
	issuebox2->add(Button("Issue #127/w", [&](auto* w) { cerr << w << ", " << getWidget("test #127/w", demo) << endl; // use `demo` yet in the print!
		getWidget<Button>("test #127/w", w)->setText("Found itself!");
	}), "test #127/w");

	//#174: std::string API
	issuebox2->add(new TextBox)->setPlaceholder("#174: std áéíÓÖŐ");


	demo.add(new Label("––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––"
	                   "(proper separators are not yet supported...)"
	                   "––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––"))->setStyle(sf::Text::Style::Italic);

	//--------------------------------------------------------------------
	// Some raw SFML shapes (sf::Text + a rect) to be manipulated via the GUI
	// + a DrawHost for boxing them nicely, as per #167

        //!! This needs to go into a w->setup() function, and actually that's where
	//!! the draw hook could also be set, optionally...
	//!! Which kinda implies two ctor forms for the two different hooks
	//!! (and fortuantely, the hook signatures differ! :) ), and of course
	//!! two setters for hooking Setup and Draw.
	//!!
	//!! Plus, a light (template-based?) interface would also be needed
	//!! for accessging them from the outside in a somewhat civilized way!
		sf::Text text(Theme::getFont(), "Hello world!");
		sf::RectangleShape textrect;

	auto sfText = new DrawHost([&](auto* raw_w, auto ctx) {
		// Get current (raw, untransformed) text size
		//! Note: the text string & style attributes may have been
		//! changed by other widgets!
		auto tbounds = text.getLocalBounds();
		//! sf::Text's bound-rect is not 0-based! :-o (-> SFML #216)
		sf::Vector2f mysterious_sfml_offset = {tbounds.left, tbounds.top};
		sf::Vector2f tsize = {tbounds.width + tbounds.left, tbounds.height + tbounds.top};
		// Get boundrect of the transformed (rotated, scaled etc.) text
		// for sizing the wrapper widget
		sf::FloatRect bgRect{{0,0}, {tsize.x * 1.2f, tsize.y * 3.5f}};
		auto actualBoundRect = text.getTransform().transformRect(bgRect);
//!!??		auto actualBoundRect = xform.transformRect(textrect.getGlobalBounds());
//!!?? Why's getGlobalBounds different here?!
//!!?? It does make the widget resize correctly -- which is the only place this
//!!?? rect is used --, but then the shape itself will stay the same size, and the
//!!?? rotation would be totally off! :-o It's all just queries! WTF?! :-ooo

		auto* w = (DrawHost*)raw_w;

		//!! Umm... This is *GROSS*! Basically triggers a full GUI resize, right from draw()! :-oo
		w->setSize((actualBoundRect.width  + 2 * Theme::PADDING),
		           (actualBoundRect.height + 2 * Theme::PADDING));

		//!! Must position the shapes after setSize, as setSize may move the widget
		//!! out from the content we manually hack here, leaving the appearance off
		//!! for the current frame! :-o (A very visible artifact, as I've learned!...)
		//!! Well, another disadvantage of calling setSize during draw()!...
		//!! Fortunately, this one can be tackled by doing this after setSize:

		// Sync the bg. rect to various "externalia":
		textrect.setScale(text.getScale());
		textrect.setRotation(text.getRotation());
		textrect.setSize(bgRect.getSize());
		textrect.setOrigin({textrect.getSize().x / 2, textrect.getSize().y / 2});
		textrect.setPosition({w->getSize().x / 2, w->getSize().y / 2});

		text.setOrigin({tsize.x / 2 + mysterious_sfml_offset.x,
                                tsize.y / 2 + mysterious_sfml_offset.y}); //! sf::Text's bound-rect is not 0-based! :-o
		text.setPosition({w->getSize().x / 2, w->getSize().y / 2});

		auto sfml_renderstates = ctx.props;
		sfml_renderstates.transform *= w->getTransform();
		ctx.target.draw(textrect, sfml_renderstates);
		ctx.target.draw(text, sfml_renderstates);
#ifdef DEBUG
		w->draw_outline(ctx);
#endif
	});


	//--------------------------------------------------------------------
	// A horizontal layout for multiple panels side-by-side
	auto main_hbox = demo.add(HBox());


	//--------------------------------------------------------------------
	// A "form" panel on the left
	auto form = main_hbox->add(Form(), "left form");

	// A text box to set the text of the example SFML object (created above)
	/*!! #171: this won't compile yet:
	form->add("Text", new sfw::TextBox())
		->setPlaceholder("Type something!")
		->set("Hello world!")
		->setCallback([&](auto* w) { text.setString(w->getString()); });
	*/
	form->add("Text", new sfw::TextBox())
		->setPlaceholder("Type something!")
		->set("Hello world!")
		->setCallback([&](auto* w) { text.setString(w->getString()); });
	// Length limit & pulsating cursor
	form->add("Text with limit (5)", new TextBox(50.f, TextBox::CursorStyle::PULSE))
		->setMaxLength(5)
		->set("Hello world!");
	//! The label above ("Text with limit (5)") will be used for retrieving
	//! the widget later by the crop slider (somewhere below)!

	// Slider + progress bars for rotating
	auto sliderForRotation = new Slider({.range = {0, 360}});
	auto pbarRotation1 = new ProgressBar(200.f, sfw::Horizontal, sfw::LabelNone);
	auto pbarRotation2 = new ProgressBar(200.f, sfw::Horizontal, sfw::LabelOver);
	auto pbarRotation3 = new ProgressBar(200.f, sfw::Horizontal, sfw::LabelOutside);
	sliderForRotation->setCallback([&](auto* w) {
		text.setRotation(sf::degrees(w->get()));
		auto pbval = w->get() / 3.6f;
		pbarRotation1->set(pbval);
		pbarRotation2->set(pbval);
		pbarRotation3->set(pbval);
	});
	// Slider + progress bars for scaling
	auto sliderForScale = new Slider({.range = {1, 3}, .step = .2f});
	auto pbarScale1 = new ProgressBar(100, sfw::Vertical, sfw::LabelNone);
	auto pbarScale2 = new ProgressBar(100, sfw::Vertical, sfw::LabelOver);
	auto pbarScale3 = new ProgressBar(100, sfw::Vertical, sfw::LabelOutside);
	sliderForScale->setCallback([&](auto* w) {
		float scale = w->get();
		text.setScale({scale, scale}); // (x, y)
		auto pbval = (w->get() - 1) / 2.f * 100;
		pbarScale1->set(pbval);
		pbarScale2->set(pbval);
		pbarScale3->set(pbval);
	});

	form->add("Rotation", sliderForRotation);
	form->add("Size", sliderForScale);

	// Color selectors
	// -- Creating one as a template to clone it later, because
	//    WIDGETS MUSTN'T BE COPIED AFTER HAVING BEEN ADDED TO THE GUI!
	OBColor ColorSelect_TEMPLATE; (&ColorSelect_TEMPLATE)
		->add("Black", sf::Color::Black)
		->add("Red", sf::Color::Red)
		->add("Green", sf::Color::Green)
		->add("Blue", sf::Color::Blue)
		->add("Cyan", sf::Color::Cyan)
		->add("Yellow", sf::Color::Yellow)
		->add("White", sf::Color::White)
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
			w->setFillColor(w->current());
		});

	// See also another one for window background color selection!

	form->add("Text color", optTxtColor);
	form->add("Text bg.", optTxtBg);

	// Checkboxes to set text properties
	// ..."classic" `add(new Widget(...))` style, setting properties in the ctor.
	// NOTE: It's generally unsafe to set the properties of a free-standing
	//       (newly created, but not-yet-added) widget!
	//       Only those supported by the constructor are guaranteed to work.
	form->add("Underlined text", new sfw::CheckBox([&](auto* w) {
		int style = text.getStyle();
		if (w->isChecked()) style |= sf::Text::Underlined;
		else                style &= ~sf::Text::Underlined;
		text.setStyle(style);
	}));
	// ..."templated" add(Widget())->set...() style, setting properties
	// on the "real" (added) widget:
	form->add("Bold text", sfw::CheckBox())->setCallback([&](auto* w) {
		int style = text.getStyle();
		if (w->isChecked()) style |= sf::Text::Bold;
		else                style &= ~sf::Text::Bold;
		text.setStyle(style);
	});

	// Attach the horiz. progress bars (used for text rotation) to the form
	form->add("H. p.bars", pbarRotation1);
	form->add("", pbarRotation2);
	form->add("", pbarRotation3);

	// Attach the vert. progress bars (used for text scaling) to a new box
	auto vbars = new sfw::HBox();
	//!! Issue #109: Document that manipulating unowned, free-standing layouts is UB!
	form->add("V. p.bars", vbars);
	vbars->add(pbarScale1);
	vbars->add(pbarScale2);
	vbars->add(pbarScale3);

/*
	auto disabled_ed = form->add("Disabled", (new TextBox)->set("Disabled"));
	cout <<	"Initial state of the \"Disabled\" widget: " << disabled_ed->enabled() << endl;
	disabled_ed->disable();
	cout  << "- after disabling: " << disabled_ed->enabled() << endl;
*/

	//--------------------------------------------------------------------
	// A panel in the middle
	auto middle_panel = main_hbox->add(sfw::VBox());

	// OK, add the SFML test shapes + adapter widget here
	middle_panel->add(sfText);

	// Button factory...
	auto boxfactory = middle_panel->add(sfw::HBox());
	auto labeller = boxfactory->add(sfw::TextBox(100))->set("Edit Me!")->setPlaceholder("Button label");
	boxfactory->add(sfw::Button("Create button", [&] {
		middle_panel->addAfter(boxfactory, new sfw::Button(labeller->get()));
	}));

	// More buttons...
	auto buttons_form = middle_panel->add(new Form);

//	buttons_form->add("Default button", Button("button"));
//	cout << "Button text retrieved via name lookup: \"" << getWidget<Button>("Default button")->getText() << "\"\n";

	auto utf8button_tag = "UTF-8 test"; // Will also use it to recall the button!
	buttons_form->add(utf8button_tag, Button("hőtűrő LÓTÚRÓ")); // Note: this source is already UTF-8 encoded.
	cout << "UTF-8 button text got back as: \"" << getWidget<Button>(utf8button_tag)->getText() << "\"\n";

	// Bitmap button
	sf::Texture buttonimg; //! DON'T put this inside the if () as local temporary (as I once have... ;) )
	if (buttonimg.loadFromFile("demo/sfmlwidgets-themed-button.png")) // SFML would print an error if failed
	{
		buttons_form->add("Native size", new sfw::ImageButton(buttonimg, "All defaults"))
			->setTextSize(20)
			->setCallback([]/*(auto* w)*/ { toy_anim_on = true; });


		buttons_form->add("Customized", new sfw::ImageButton(buttonimg, "Bold"))
			->setTextSize(20)
			->setTextStyle(sf::Text::Style::Bold)
			->setSize({180, 35})
			->setTextColor(hex2color("#d0e0c0"))
			->setCallback([]/*(auto* w)*/ { toy_anim_on = false; });
	}


	//--------------------------------------------------------------------
	// Image views...

	// Image directly from file
	auto vboximg = main_hbox->add(sfw::VBox());
	vboximg->add(sfw::Label("Image from file:"));
	vboximg->add(sfw::Image("demo/some image.png"));

	// Image crop from file
	vboximg->add(sfw::Label("Crop from file:"));
	vboximg->add(sfw::Image("demo/some image.png", {{0, 33}, {24, 28}}));
	vboximg->add(sfw::Label("Image crop varied:"));

	// Image from file, cropped dynamically
	sfw::Image* imgCrop = new sfw::Image("demo/martinet-dragonfly.jpg");

	// Slider & progress bar for cropping an Image widget
	vboximg->add(Slider({}, 100))->setCallback([&](auto* w) {
		getWidget<ProgressBar>("cropbar")->set(w->get());
		// Show the slider value in a text box retrieved by its name:
		auto tbox = getWidget<TextBox>("Text with limit (5)");
		if (!tbox) cerr << "Named TextBox not found! :-o\n";
		else tbox->set(to_string((int)w->get()));
		imgCrop->setCropRect({{(int)(w->get() / 4), (int)(w->get() / 10)},
		                      {(int)(w->get() * 1.4), (int)w->get()}});
	});

	auto boxcrop = vboximg->add(new sfw::HBox);
		boxcrop->add(sfw::Label("Crop square size:"));
		boxcrop->add(sfw::ProgressBar(40), "cropbar");

	vboximg->add(imgCrop);
	vboximg->add(sfw::Label("(Art: © Édouard Martinet)"))->setStyle(sf::Text::Style::Italic);


	//--------------------------------------------------------------------
	// Another "sidebar" column, for (theme) introspection...
	auto right_bar = main_hbox->add(sfw::VBox());

	// Theme selection
	using OBTheme = sfw::OptionsBox<Theme::Cfg>;
	right_bar->add(sfw::Label("Change theme:"));
	auto themeselect = new OBTheme([&](auto* w) {
		const auto& themecfg = w->current();
		demo.setTheme(themecfg); // Swallowing the error for YOLO reasons ;)
		// Update the wallpaper on/off checkbox:
		if (getWidget("Wallpaper")) getWidget<CheckBox>("Wallpaper")->set(demo.hasWallpaper());
	});
	for (auto& t: themes) { themeselect->add(t.name, t); }
	themeselect->set(DEFAULT_THEME);
	right_bar->add(themeselect, "theme-selector");

	// Theme font size slider
	// (Also directly changes the font size of the theme cfg. data stored in the
	// "theme-selector" widget, so that it remembers the updated size (for each theme)!)
	right_bar->add(Label("Theme font size (use the m. wheel):"));
	right_bar->add(Slider({.range = {8, 18}}, 100))
		->set((float)themes[DEFAULT_THEME].textSize)
		->setCallback([&] (auto* w){
			assert(getWidget("theme-selector"));
			auto& themecfg = getWidget<OBTheme>("theme-selector")->currentRef();
			themecfg.textSize = (size_t)w->get();
cerr << "font size: "<< themecfg.textSize << endl; //!!#196
			demo.setTheme(themecfg);
		});

	// Show the current theme texture bitmaps
	right_bar->add(sfw::Label("Theme textures:"));
	auto txbox = right_bar->add(new HBox);
	struct ThemeBitmap : public Image {
		ThemeBitmap() : Image(Theme::getTexture()) {}
		void onThemeChanged() override { setTexture(Theme::getTexture()); } // note: e.g. the ARROW is at {{0, 42}, {6, 6}}
	};
	auto themeBitmap = new ThemeBitmap; //ThemeBitmap(2); // start with 2x zoom
	txbox->add(Slider({.range = {1, 5}, .orientation = Vertical, .invert = true}, 100))
		->setCallback([&](auto* w) { themeBitmap->scale(w->get()); })
		->setIntervals(2) // divide the range into 2 intervals, yielding stops at: 1, 3, 5
		->update(4.f); // use update(), not set(), to trigger the callback!
	txbox->add(themeBitmap);

	right_bar->add(Label(" ")); // Just for some space, indeed...

	// Playing with the background...
	auto bgform = right_bar->add(new Form);

	// Wallpaper on/off checkbox
	bgform->add("Wallpaper", sfw::CheckBox([&](auto* w) { w->checked() ? demo.setWallpaper() : demo.disableWallpaper(); },
	                                       demo.hasWallpaper()));
	// Wallpaper transparency slider
	bgform->add("Wallpaper α", Slider({.range={0, 255}}, 75))
		->set(demo.getWallpaper().getColor().a)
		->setCallback([&](auto* w) {
			assert(getWidget("theme-selector"));
			auto& themecfg = getWidget<OBTheme>("theme-selector")->currentRef();
			themecfg.wallpaper.tint = {themecfg.wallpaper.tint.r,
			                           themecfg.wallpaper.tint.g,
			                           themecfg.wallpaper.tint.b,
			                           (uint8_t)w->get()};
			demo.setWallpaperColor(themecfg.wallpaper.tint);
		});
//cerr << "wallpap alpha: " << getWidget<Slider>("Wallpaper α")->get() << endl;
	// Window background color selector
	bgform->add("Window bg.", (new OBColor(ColorSelect_TEMPLATE))
		->add("Default", themes[DEFAULT_THEME].bgColor)
		->setCallback([&](auto* w) {
			Theme::bgColor = w->current();
		})
	);

	// A pretty useless, but interesting clear-background checkbox
	auto hbox4 = right_bar->add(new HBox);
	hbox4->add(Label("Clear background"));
	// + a uselessly convoluted name-lookup through its own widget pointer, to confirm
	// the get() name fix of #200 (assuming CheckBox still has its "real" get()):
	hbox4->add(CheckBox([&](auto* w) { Theme::clearBackground = getWidget<CheckBox>("findme", w)->get(); },
	                    true), "findme");

	auto disable_all_box = right_bar->add(new HBox);
	disable_all_box->add(Label("Disable/Enable all"));
	disable_all_box->add(CheckBox([&](auto* chkbox) {
		demo.foreach([&](auto* widget) {
			if (widget != chkbox) widget->enable(chkbox->get()); // Leave the checkbox alive! :)
		});
	}, true ));


	// "GUI::close" button -- should NOT close the window:
	demo.add(sfw::Button("Close (deactivate)!", [&] { demo.close(); }))
		->setTooltip("Hello!");

	// Set this last, otherwise the dynamic GUI resize (on adding new widgets)
	// may interfere with it!
	//demo.setWallpaper("asset/wallpaper.jpg");
//	demo.setWallpaper();

	// Test the duplicate naming warning:
	demo.setName("demo"); demo.setName("demo");
	cerr << "(Should see a warning about dup. reg. above.)\n";


	//--------------------------------------------------------------------
	// OK, GUI Setup done. Set some "high-level" defaults
	// (after setup, as these may trigger callbacks)
	//
	sliderForRotation->set(97);
	sliderForScale->set(1.2f);
	//!!#160, too:
	optTxtColor->select("Red"); // Now all ready, safe to trigger the update callback (so, not just set()...)
	optTxtBg->select("Black");

	//--------------------------------------------------------------------
	// Start another thread for some unrelated job
	thread bg_thread(background_thread_main, std::ref(demo));

	//--------------------------------------------------------------------
	// The event loop
	while (window.isOpen())
	{
		demo.render();
		window.display();

		sf::Event event;
		// Pass events to the GUI & check for closing or failure
		//!! Can't shortcut it when the GUI doesn't own the window, so when
		//!! it gets deactivated it wouldn't block the rest of the world, too:
		//!!while (window.pollEvent(event) && demo.process(event) || )
		while (window.pollEvent(event))
		{
			demo.process(event);

			// Handle the window closing separately, as we
			// don't want to close with the gui in this setup:
			if (event.type == sf::Event::Closed ||
			    // Just for convenience, also close on Esc:
			    (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
			{
				//demo.close(); // No need to close it manually: window.close() will make it inactive!
				window.close(); // Not demo.close if the GUI doesn't own the window!
			}
		}
	}

	//--------------------------------------------------------------------
	// Finish the bg. thread, too
	bg_thread.join();

	return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------
void background_thread_main(sfw::GUI& gui)
{
	auto rot_slider = getWidget<Slider>("Rotation", gui);
	if (!rot_slider)
		return;

	for (size_t n = 0; gui;)
	{
		// Cycle the rot. slider
	        auto sampletext_angle = sf::degrees(rot_slider->get() * 3 + 4);
		rot_slider->update(float(int(sampletext_angle.asDegrees()/3) % 100));

		// Keep on sleeping while the anim. is disabled.
		// But still also poll the GUI for termination.
		// NOTE: it's important to do this AFTER manipulating the widget
		// exactly because the GUI may have been shut down in the meantime,
		// and it's unhealthy (UB) to still use it.
		do this_thread::sleep_for(chrono::milliseconds(50));
		while (!toy_anim_on && gui);

		++n;
		if (n%20) continue;
		gui.setPosition(float(10 + (n/20)%10), float(10 + (n/20)%10));
	}
}
