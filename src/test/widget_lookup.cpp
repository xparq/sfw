#include "sfw/GUI.hpp"
#include "sfw/WidgetLiteral.hpp" //!!EXPERIMENTAL!!

#include <SFML/Graphics.hpp>

#include <string> // to_string
#include <iostream> // cerr, for errors, cout for some "demo" info

#include "SAL/util/diagnostics.hpp"

using namespace std;
using namespace sfw::geometry;

// Try/check without most of the sfw:: prefixes:
using namespace sfw;


//============================================================================
int main()
try {
	//------------------------------------------------------------------------
	// SFML app frame....
	//------------------------------------------------------------------------

	sf::RenderWindow window;
	window.create(sf::VideoMode({1024, 768}), "SFW TEST: widget lookup & access", sf::Style::Close|sf::Style::Resize);
	if (!window.isOpen()) {
		cerr << "- Failed to create the SFML window!\n";
		return EXIT_FAILURE;
	}
	window.setFramerateLimit(30);

	//------------------------------------------------------------------------
	// Setup the test GUI...
	//------------------------------------------------------------------------

	using OBColor = OptionsBox<Color>;

	Theme::PADDING = 2.f;
	Theme::click.textColor      = Color("#191B18");
	Theme::click.textColorHover = Color("#191B18");
	Theme::click.textColorFocus = Color("#000");
	Theme::input.textColor      = Color("#000");
	Theme::input.textColorHover = Color("#000");
	Theme::input.textColorFocus = Color("#000");
	Theme::input.textColorDisabled = Color("#888");
	Theme::input.textSelectionColor = Color("#97b1AD");
	Theme::input.textPlaceholderColor = Color("#8791AD");

	// Some dynamically switchable theme "quick config packs" to play with
	Theme::Cfg themes[] = {
		{ "Baseline", "demo/", "texture-sfw-baseline.png", Color("#e6e8e0"),
		  Wallpaper::Cfg("demo/wallpaper.jpg", Wallpaper::Center, Color(255,25,25,20)), 11, "font/LiberationSans-Regular.ttf" },
		{ "Classic ☺", "demo/", "texture-sfw-classic.png",  Color("#e6e8e0"), {}, 12, "font/LiberationSans-Regular.ttf" },
		{ "\"factory default\"", },
	};
	const size_t DEFAULT_THEME = 0;


	//--------------------------------------------------------------------
	// Creating the main GUI controller:
	GUI demo(window, themes[DEFAULT_THEME],
		false); // <- Don't manage the window (e.g. don't .clear() it, only the GUI rect)!
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
		sfw::gfx::Texture backdrop;
                if (!backdrop.load("test/example.jpg")) return;
		circ.setTexture(&backdrop); //!! No need for circ.setTexture(&backdrop.native()), as sf::Texture is a mixin of sfw::gfx::Texture!
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
	auto issue_171 = test_hbox->add(new Form);
	issue_171->add("#171 autocast", new TextBox(50))->set("OK!");
	//#171 + #168 (with an Image*):
	issue_171->add((new Image("test/example.jpg"))->rescale(0.25), new TextBox(50))->set("171 + 168 OK!");
	//#171 + #168 + template widget + move
	issue_171->add(new Label("tempWidget"), TextBox(70))->set("171/tmp + 168 OK!");
	//#171, template-move also for the "label":
	issue_171->add(Label("tmpLabel"), Label("#171(tmp,tmp) OK too!"));

	//!! This is not yet supported (nor separators...):
	//!!test_hbox->add(new Form)->add("This is just some text on its own.");

	auto issues_2 = test_hbox->add(new VBox);

	//
	// #127 + name lookup...
	//
	issues_2->add(Button("Issue #127/void", [] {
		// ...via the main widget manager (`demo`):
//OLD:		getWidget<Button>("test #127", demo)->setText("Found itself!");
		call<Button>("test #127", [](auto* w){ w->setText("Found itself!"); });
	}), "test #127");

	issues_2->add(Button("Issue #127/w", [&](auto* w) {
//cerr <<"- Type of 'auto' widget in callback:" << typeid(decltype(w)).name() << "\n";
//OLD:		cerr << w << ", " << getWidget("test #127/w", demo) << endl;
//NEW, awkward (needs capturing `demo`):
//!!		cerr << w << ", " << demo.find_widget("test #127/w") << endl;
//NEW, OK:
		cerr << w << ", " << w->find_widget("test #127/w") << endl;
//!! All these suffer from the "uncheckable null-> crash" problem...:
//OLD:		getWidget<Button>("test #127/w", w)->setText("Found itself!");
//NEW, awkward (needs capturing the manager instance (`demo`)):
//!!		demo.find_widget<Button>("test #127/w")->setText("Found itself!");
//!! Not even this (at least to spare the capture):
//!!		w->getMain()->find_widget<Button>("test #127/w")->setText("Found itself!");
//!! Alas, not (neither syntactically, for a sad C++ parsing limitation, nor for null->...):
//!!		w->find_widget<Button>("test #127/w")->setText("Found itself!"); //!!C++: fails if w is `auto`! :-/
//!! The syntax only works this pathetic way:
//		w->template find_widget<Button>("test #127/w")->setText("Found itself!");
//!! - MSVC can actually do it! (GCC 13.2 can't, no idea about CLANG; also, MSVC may be non-std here! :-/ ):
//		w->find_widget<Button>("test #127/w")->setText("Found itself!");
//!! But the null-> is still the same old problem:
//!!		w->template find_widget<Button>("weee")->setText("Busted!");
//!! So, any ...-> still needs explicit checking currently, and, sadly,
//!! this lame, tedious form is kinda the best C++ can offer:
//		if (auto b = (Button*)w->widget("test #127/w"); b) b->setText("Found itself!");
//!! Even this fails with GCC, on the same C++ template-parsing awkwardness as above (if w is declared `auto`!):
//!! (And BTW, the method-passing technique is flawed anyway, for the "hostility" of C++ defaults...
//!! (I.e. if a) methods could be passed by reference instead of pointer only, and b) would need no
//!! disambiguation in the 99% of cases when they aren't overloaded, it could be a viable approach.))
//!!#ifdef _MSC_VER
//!!		w->widget("test #127/w").call<&Button::setText>("Found itself!");
//!!#else
//!! This abomination again...:
//		w->template widget("test #127/w").call<&Button::setText>("Found itself!");
//!! Or, back to macros...
		// Denoised (but unfortunately also destructured) syntax:
		SFW_CALL(Button, "test #127/w", setText, "Found itself!");
			//!! Should even be unified, with the trick of having a static sfw::getWidget as well as w->getWidget! :)
			//!!w->SFW_CALL(Button, "test #127/w", setText, "Found itself!");
//!!#endif

	}), "test #127/w");

	// #174: std::string API
	issues_2->add(new TextBox)->setPlaceholder("#174: std áéíÓÖŐ");

	auto issues_3 = test_hbox->add(new Form);
	// #333: Color() with alpha, #335: Form separator lines
	issues_3->add("Shouldn't chg. (no α vs ff):"); // #335 sep. line
	issues_3->add("#333 h2c(α)", CheckBox([](auto* w){
		Theme::bgColor = Color(w->checked() ? "#ff5566" :"#ff5566ff"); }));
	issues_3->add("Should change (α = 0 vs 80):"); // #335 sep. line
	issues_3->add("#333 h2c(α)", CheckBox([](auto* w){
		Theme::bgColor = Color(w->checked() ? "#ff556600" : "#ff556680"); }));
	issues_3->add("", new Label("#335: empty label OK")); // Still on the right side?


	// #347...
	auto issue_347 = test_hbox->add(new Form);
	issue_347->add("guarded box", TextBox(50));
	

	demo.add(new Label("––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––" //! UNICODE U+2013, not just '-'! ;-o
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
		sfw::gfx::Text text("Hello world!");
		sf::RectangleShape textrect;

	auto sfText = new DrawHost([&](auto* raw_w, auto ctx) {
		// Get current (raw, untransformed) text size
		//! Note: the text string & style attributes may have been
		//! changed by other widgets!
		auto tsize = text.size();
		// Get boundrect of the transformed (rotated, scaled etc.) text
		// for sizing the wrapper widget
		fRect bgRect{{}, {tsize.x() * 1.2f, tsize.y() * 3.5f}};
		auto actualBoundRect = text.getTransform().transformRect(bgRect);
//!!??		auto actualBoundRect = xform.transformRect(textrect.getGlobalBounds());
//!!?? Why's getGlobalBounds different here?!
//!!?? It does make the widget resize correctly -- which is the only place this
//!!?? rect is used --, but then the shape itself will stay the same size, and the
//!!?? rotation would be totally off! :-o It's all just queries! WTF?! :-ooo

		auto* w = (DrawHost*)raw_w;

		//!! Umm... This is *GROSS*! Basically triggers a full GUI resize, right from draw()! :-oo
		w->setSize(fVec2(actualBoundRect.size) + 2.f * Theme::PADDING);

		//!! Must position the shapes after setSize, as setSize may move the widget
		//!! out from the content we manually hack here, leaving the appearance off
		//!! for the current frame! :-o (A very visible artifact, as I've learned!...)
		//!! Well, another disadvantage of calling setSize during draw()!...
		//!! Fortunately, this one can be tackled by doing this after setSize:

		// Sync the bg. rect to various "externalia":
		textrect.setScale(text.getScale());
		textrect.setRotation(text.getRotation());
		textrect.setSize(bgRect.size());
		textrect.setOrigin({textrect.getSize().x / 2, textrect.getSize().y / 2});
		textrect.setPosition({w->getSize().x() / 2, w->getSize().y() / 2});

		text.setOrigin(tsize / 2);
		text.position(w->getSize() / 2);

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
	form->add("Text", new TextBox())
		->setPlaceholder("Type something!")
		->set("Hello world!")
		->setCallback([&](auto* w) { text.setString(w->getString()); });
	*/
	form->add("Text", new TextBox())
		->setPlaceholder("Type something!")
		->set("Hello world!")
		->setCallback([&](auto* w) { text.set(w->get()); });
	// Length limit & pulsating cursor
	form->add("Text with limit (5)", new TextBox(50.f, TextBox::CursorStyle::PULSE))
		->setMaxLength(5)
		->set("Hello world!");
	//! The label above ("Text with limit (5)") will be used for retrieving
	//! the widget later by the crop slider (somewhere below)!

	// Slider + progress bars for rotating
	auto sliderForRotation = new Slider({.range={0, 360}});
	auto pbarRotation1 = new ProgressBar({.length=200, .range={0, 360}, .label_placement=LabelNone});
	auto pbarRotation2 = new ProgressBar({.length=200, .range={0, 360}, .unit="°", .label_placement=LabelOver});
	auto pbarRotation3 = new ProgressBar({.length=200, .range={0, 100}, .clamp=false, //! Test no-clamp, too!
	                                                                    .unit="°", .label_placement=LabelOutside});
	sliderForRotation->setCallback([&](auto* w) {
		auto degrees = w->get();
		text.setRotation(sf::degrees(degrees));
		pbarRotation1->set(degrees);
		pbarRotation2->set(degrees);
		pbarRotation3->set(degrees);
	});
	// Slider + progress bars for scaling
	auto sliderForScale = new Slider({.range = {1, 3}, .step = .2f});
	auto pbarScale1 = new ProgressBar(100, Vertical, LabelNone);
	auto pbarScale2 = new ProgressBar(100, Vertical, LabelOver);
	auto pbarScale3 = new ProgressBar(100, Vertical, LabelOutside);
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
		->add("Black",  Color::Black)
		->add("Red",    Color::Red)
		->add("Green",  Color::Green)
		->add("Blue",   Color::Blue)
		->add("Cyan",   Color::Cyan)
		->add("Yellow", Color::Yellow)
		->add("White",  Color::White)
	;

	// Select sample text color
	auto optTxtColor = (new OBColor(ColorSelect_TEMPLATE))
		->setCallback([&](auto* w) {
			text.setFillColor(w->get());
			w->setTextColor(w->get());
		});

	// Select sample text box color
	auto optTxtBg = (new OBColor(ColorSelect_TEMPLATE))
		->add("Default", themes[DEFAULT_THEME].bgColor)
		->setCallback([&](auto* w) {
			textrect.setFillColor(w->get());
			w->setFillColor(w->get());
		});

	// See also another one for window background color selection!

	form->add("Text color", optTxtColor);
	form->add("Text bg.", optTxtBg);

	// Checkboxes to set text properties
	// ..."classic" `add(new Widget(...))` style, setting properties in the ctor.
	// NOTE: It's generally unsafe to set the properties of a free-standing
	//       (newly created, but not-yet-added) widget!
	//       Only those supported by the constructor are guaranteed to work.
	form->add("Underlined text", new CheckBox([&](auto* w) {
		int style = text.getStyle();
		if (w->isChecked()) style |= sf::Text::Underlined;
		else                style &= ~sf::Text::Underlined;
		text.setStyle(style);
	}));
	// ..."templated" add(Widget())->set...() style, setting properties
	// on the "real" (added) widget:
	form->add("Bold text", CheckBox())->setCallback([&](auto* w) {
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
	auto vbars = new HBox();
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
	auto middle_panel = main_hbox->add(VBox());

	// OK, add the SFML test shapes + adapter widget here
	middle_panel->add(sfText);

	// Button factory...
	auto boxfactory = middle_panel->add(HBox());
	auto labeller = boxfactory->add(TextBox(100), "editme")->set("Edit Me!")->setPlaceholder("Button label");
	boxfactory->add(Button("Create button", [&] {
		middle_panel->addAfter(boxfactory, new Button(labeller->get()));
	}));

	// More buttons...
	auto buttons_form = middle_panel->add(new Form);

//	buttons_form->add("Default button", Button("button"));
//	cout << "Button text retrieved via name lookup: \"" << getWidget<Button>("Default button")->getText() << "\"\n";

	auto utf8button_tag = "UTF-8 test"; // Will also use it to recall the button!
	buttons_form->add(utf8button_tag, Button("hőtűrő LÓTÚRÓ")); // Note: this source is already UTF-8 encoded.
	cout << "UTF-8 button text got back as: \"" << widget<Button>(utf8button_tag)->getText() << "\"\n";


	//--------------------------------------------------------------------
	// Image views...

	auto vboximg = main_hbox->add(VBox());
	vboximg->add(Label("Image crop varied:"));

	Image* imgCrop = new Image("demo/martinet-dragonfly.jpg");

	// Slider & progress bar for cropping an Image widget
	vboximg->add(Slider({.length = 100}), "cropsize")->setCallback([&](auto* w) {
// UNCHECKED:
		demo.find_widget<ProgressBar>("cropbar")->set(w->get());
// UNCHECKED, VIA THE DEFAULT MGR.:
		widget<ProgressBar>("cropbar")->set(w->get());
// OK:
		demo.set<ProgressBar>("cropbar", w->get());
		
		// Show the slider value in a text box retrieved by its name:
		auto tbox = widget<TextBox>("Text with limit (5)");
		if (!tbox) cerr << "Named TextBox not found! :-o\n";
		else tbox->set(to_string((int)w->get()));
		imgCrop->setCropRect({{(int)(w->get() / 4), (int)(w->get() / 10)},
		                      {(int)(w->get() * 1.4), (int)w->get()}});
	});

	auto boxcrop = vboximg->add(new HBox);
		boxcrop->add(Label("Crop square size:"));
		boxcrop->add(ProgressBar(40), "cropbar");

	vboximg->add(imgCrop);

	//--------------------------------------------------------------------
	// Another "sidebar" column, for (theme) introspection...
	auto right_bar = main_hbox->add(VBox());

	// Theme selection
	using OBTheme = OptionsBox<Theme::Cfg>;
	right_bar->add(Label("Change theme:"));
	auto themeselect = new OBTheme([&](OBTheme* w) {
		demo.setTheme(w->get()); //! Swallowing the error for YOLO reasons...
		// Update the wallpaper on/off checkbox:
//OLD:
//		if (getWidget("Wallpaper")) getWidget<CheckBox>("Wallpaper")->set(demo.hasWallpaper());
//NEW:
//!!C++ bullshit: can't just use this form:
//!!		if (auto wp = w->find_widget<CheckBox>("Wallpaper"); wp) wp->set(demo.hasWallpaper());
//!!because the std. mandates it to be this ridiculous nonsense:
//!!		if (auto wp = w->template find_widget<CheckBox>("Wallpaper"); wp) wp->set(demo.hasWallpaper());
//!!it only works, if the callback lambda arg. above is not `auto* w`, but explicitly `OBTheme* w`:
//!!		if (auto wp = w->find_widget<CheckBox>("Wallpaper"); wp) wp->set(demo.hasWallpaper());
//OR:
		demo.call<CheckBox>("Wallpaper", [&](auto* wp) { wp->set(demo.hasWallpaper()); });
//OR:
		call<CheckBox>("Wallpaper", [&](auto* wp) { wp->set(demo.hasWallpaper()); });
//OR:
		demo.SFW_SET(CheckBox, "Wallpaper", demo.hasWallpaper());
//OR:
		SFW_SET(CheckBox, "Wallpaper", demo.hasWallpaper());
//OR:
		demo.set<CheckBox>("Wallpaper", demo.hasWallpaper());
//OR:
		set<CheckBox>("Wallpaper", demo.hasWallpaper());
	});
	for (auto& t: themes) { themeselect->add(t.name, t); }
	themeselect->set(DEFAULT_THEME);
	right_bar->add(themeselect, "theme-selector");

	// Theme font size slider
	// (Also directly changes the font size of the theme cfg. data stored in the
	// "theme-selector" widget, so that it remembers the updated size (for each theme)!)
	right_bar->add(Label("Theme font size (use the m. wheel):"));
	right_bar->add(Slider({.length = 100, .range = {8, 18}}))
		->set((float)themes[DEFAULT_THEME].fontSize)
		->setCallback([&] (auto* w){
#if 0 // OLD
			assert(demo.find_widget("theme-selector")); // Freak out and die in DEBUG mode.
			if (auto ts = demo.find_widget<OBTheme>("theme-selector"); ts) { // Check silently in non-DEBUG.
				auto& themecfg = ts->current(); // or, synonym: ->get();
				themecfg.fontSize = (size_t)w->get();
cerr << "font size (method 1): "<< themecfg.fontSize << endl; //!!#196
				demo.setTheme(themecfg);
			}
#elif 0 // NEW, BUT EVEN MORE CUMBERSOME
			if (auto res = demo.call<OBTheme>("theme-selector", [](auto* ts) { return ts->get(); }); res) {
				auto& themecfg = res.value();
				themecfg.fontSize = (size_t)w->get();
cerr << "font size (method 2): "<< themecfg.fontSize << endl; //!!#196
				demo.setTheme(themecfg);
			}
#elif 1 // NEW, WITH get<> -- AWKWARD IN ANOTHER WAY (needs access to the default theme cfg.; can't do ref. for the default, and falls back to it with different semantics!)
			auto themecfg = demo.get<OBTheme>("theme-selector", themes[DEFAULT_THEME]); // NOTE: not a ref; can't write to the result directly!
			themecfg.fontSize = (size_t)w->get();
cerr << "font size (method 3): "<< themecfg.fontSize << endl; //!!#196
			demo.setTheme(themecfg);
#elif 0 // NEW, LEAN
			demo.call<OBTheme>("theme-selector", [&](auto* ts) {
				auto& themecfg = ts->get();
				themecfg.fontSize = (size_t)w->get();
cerr << "font size (method 4): "<< themecfg.fontSize << endl; //!!#196
				demo.setTheme(themecfg);
			});
#endif				
		});

	// Show the current theme texture bitmaps
	right_bar->add(Label("Theme textures:"));
	auto txbox = right_bar->add(new HBox);
	struct ThemeBitmap : public Image {
		ThemeBitmap() : Image(Theme::getTexture()) {}
		void onThemeChanged() override { setTexture(Theme::getTexture()); } // note: e.g. the ARROW is at {{0, 42}, {6, 6}}
	};
	auto themeBitmap = new ThemeBitmap; //ThemeBitmap(2); // start with 2x zoom
	txbox->add(Slider({.length = 100, .range = {1, 5}, .orientation = Vertical, .invert = true}))
		->setCallback([&](auto* w) { themeBitmap->scale(w->get()); })
		->setIntervals(2) // divide the range into 2 intervals, yielding stops at: 1, 3, 5
		->update(4.f); // use update(), not set(), to trigger the callback!
	txbox->add(themeBitmap);

	right_bar->add(Label(" ")); // Just for some space, indeed...

	// Playing with the background...
	auto bgform = right_bar->add(new Form);

	// Wallpaper on/off checkbox
	bgform->add("Wallpaper", CheckBox([&](auto* w) { w->checked() ? demo.setWallpaper() : demo.disableWallpaper(); },
	                                       demo.hasWallpaper()));
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
	hbox4->add(CheckBox([&](Widget* w) { //!!CPP: Widget*, not auto*, to spare the `w->template get/set` nonsense below.
//! OLD, CLUMSY, AND ALSO UNCHECKED:
				assert(w->find_widget("findme"));
				Theme::clearBackground = w->find_widget<CheckBox>("findme")->get();
//! CLEANEST:
				Theme::clearBackground = w->get_or<CheckBox>("findme", true);
//! OR (A SYNONYM):
				Theme::clearBackground = w->get<CheckBox>("findme", true);
//! COMPILE-CHECKING RAW get<> TOO:
				Theme::clearBackground = w->template get<CheckBox>("findme").value();
//! JUST DOUBLE-CHECKING IF set<> ALSO COMPILES:
				w->set<CheckBox>("findme", Theme::clearBackground);
	                    },
	                    true),
	           "findme"
	);

	auto disable_all_box = right_bar->add(new HBox);
	disable_all_box->add(Label("Disable/Enable all"));
	disable_all_box->add(CheckBox([&](auto* chkbox) {
		demo.traverse([&](auto* widget) {
			if (widget != chkbox) widget->enable(chkbox->get()); // Leave the checkbox alive! :)
		});
	}, true ));


	//--------------------------------------------------------------------
	// OK, GUI setup done. Update some initial widget states post-setup,
	// triggering callbacks (now that we can)! (Unlike using just set()
	// in the setup phase above.)
	//
	sliderForRotation->update(97);
	sliderForScale->update(1.2f);
	//!!#160, too:
	optTxtColor->select("Red"); // Now all ready, safe to trigger the update callback
	optTxtBg->select("Black");

	//--------------------------------------------------------------------
	// Guarded widget access...
	//

	// Proxied updates:
	demo.update<Slider>("cropsize", 40);
	update<Slider>("cropsize", 50);

	// Proxied sets (no update-callback):
	// Doing the sets after the updates so the widget states can reflect
	// the results of both: the cropped image should be half its size now,
	// but the slider should be at 90% after these:
	demo.set<Slider>("cropsize", 80);
	set<Slider>("cropsize", 90);


//!!C++: Nope... :-/ Can't deduce template arg. from a lambda arg:
//!!	demo.call("guarded box", [](TextBox* w){w->set("OK!");});
//!! Only:
	demo.call<TextBox>("guarded box", [](auto w){w->set("OK!");});

/*!! THESE FORMS, TOO:
	demo.xxx<TextBox>("guarded box", SFW_CALL(set, "OK!"));
	demo.SFW_CALL(TextBox, "guarded box", set, "OK!");
	SFW_CALL(TextBox, "guarded box", set, "OK!");
	w->SFW_CALL(TextBox, "guarded box", set, "OK!");

//!! Alas, the param. list of these would look awkward...:
//!!	demo.w_set<TextBox>("guarded box", "OK!");
//!!	cerr << demo.w_get<TextBox>("guarded box", "default");
!!*/
//	sfw::apply<&TextBox::set>("guarded box", "It works!");

	//!!EXPERIMENTAL!!
	// Suboptimal (-> that cast...) tricks with a user-defined string literal:
	((TextBox*)"guarded box"_W)->set("It works!");
	((TextBox&)"guarded box"_W).set("It works!");
//!! Alas, can't call magically makeit a TextBox just by typing the callback arg.:
//!!	"guarded box"_W.call([](TextBox* w){w->setMaxLength(8);});

	//!!EXPERIMENTAL: Guarded & typed widget op. via the default (singleton) GUI controller: 
	widget<TextBox>("guarded box").call([](auto* w){w->setMaxLength(8);});

	demo.call<TextBox>("guarded box", [](auto* w){w->set("It works!");});
	// Or:
	demo.SFW_CALL(TextBox, "guarded box", set, "It works!");
	// Or:
	demo.SFW_SET(TextBox, "guarded box", "It works!");
	// Or:
	set<TextBox>("guarded box", "It works!");
	// Or could be:
	//	W("guarded box").call<&TextBox::set>("It works!");
	//	call<"guarded box"_W, &TextBox::set>("It works!");

	//!! Can't do this, though: "guarded box"_W<TextBox>->set("It works!");

	//!! Can't smuggle that <CheckBox> over to WidgetPtr this way! :-/
	//!!	with<CheckBox>(WidgetPtr("findme"), &CheckBox::set, false); // free-func getter

	//- - - - -
	// get/set...

	set<CheckBox>("findme", true);
	// Or, explicitly proxied:
	demo.SFW_SET(CheckBox, "findme", true);

	cerr	<< "Checkbox state - unchecked, because known to exist (should be `true`): "
		<< get_v<CheckBox>("findme")
		<< '\n'; // free-func getter

	cerr	<< "Checkbox state - guarded: use default if doesn't exist (but it does, should be `true`): "
		<< get_or<CheckBox>("findme", false)
		<< '\n'; // free-func getter

	cerr	<< "Checkbox state - guarded: use default value if doesn't exist (should be `false`): "
		<< get_or<CheckBox>("can't find me", false)
		<< '\n'; // free-func getter

	cerr	<< "Checkbox state - explicitly error-checked result (should be `true`): ";
	if (auto res = get<CheckBox>("findme"); res)
		cerr << res.value() << '\n';
	else
		cerr << "- this widget doesn't exist!\n";

	cerr	<< "State of a nonexistent slider (should be empty, plus error(s)...):\n";
	if (auto result = get<Slider>("*this widget does not exist*"); result) {
		cerr << "- WOW: this widget still exists?! WTF???\n";
	} else {
		cerr << "- OK, this widget really doesn't exist.\n";
	}

/*!!
	with("editme", &TextBox::set, "hijacked! :-o");
	//!! OTOH, alas, the templated update(...) won't compile here:
	//!!with(getWidget<TextBox>("editme"), &TextBox::set, "hijacked! :-o");
	//!!with(getWidget<TextBox>("editme"), &TextBox::update<TextBox>, "hijacked! :-o");
!!*/
	auto save_old = get_or<TextBox>("editme", "<NOT FOUND! WTF?>");
	     save_old = get<TextBox>("editme", "<NOT FOUND! WTF?>"); //! Compile-checking its synonym...
/*!!
	with("editme", &TextBox::set, "hijacked! :-o");
	cout	<< "SFW_QUERY_WITH_DEFAULT(TextBox, \"editme\", get...), should be \"hijacked! :-o\": "
		<< SFW_QUERY_WITH_DEFAULT(TextBox, "editme", get, "<WIDGET NOT FOUND!>") << '\n';

	with("editme", &TextBox::set, save_old);
!!*/
	SFW_SET(TextBox, "editme", save_old); // Redundant; just verifying that it compiles.
	 	//!! Also implement w->SFW_SET(...)!

	//- - - - -
	// get/set -- nonexistent, via specific widget...
	//

	// Should trigger "warning: ignoring return value...":
	//demo.get<TextBox>("!!! FAKE 1 - UNSAFE/UNCHECKED, also discarded !!!");

	std::ignore = demo.get<TextBox>("!!! FAKE 1 - UNSAFE/UNCHECKED (but ignored) !!!");

	cout << "Content of nonexistent textbox: "
	     << demo.get_or<TextBox>("!!! FAKE 2 !!!", "<WIDGET NOT FOUND!>") << '\n';

	demo.set<TextBox>("!!! FAKE 3 !!!", "crap");

	//- - - - -
	// get/set -- nonexistent, via the static singleton GUI mgr...
	//

	// Should trigger "warning: ignoring return value...":
	// get<TextBox>("!!! FAKE 1 - UNSAFE/UNCHECKED, also discarded !!!");

	std::ignore = get<TextBox>("!!! FAKE 1 - UNSAFE/UNCHECKED (but ignored) !!!");

	cout << "Content of nonexistent textbox: "
	     << get_or<TextBox>("!!! FAKE 2 !!!", "<WIDGET NOT FOUND!>") << '\n';

	set<TextBox>("!!! FAKE 3 !!!", "crap");

	//!! Wet-dream proxy classes... -- surprise, surprise: not really feasible in C++. :-/
	//!! It would require either an universal dummy "null" instance for every widget class,
	//!! safe to call with any of their methods (+ the burden of a dedicated uniform ctor),
	//!! or: exceptions, to break out from the closed op-> chain (generated by the compiler)!
	//!!WidgetPtr<CheckBox>("findme")->set(true);
	//!!WidgetRef<CheckBox>("findme").set(true);
	//!!WidgetRef<CheckBox>("findme").get_or(false);
	//!!Widget<CheckBox>("findme").set(true); // Perhaps even this, for the fearless!... ;)
	//!!Widget<CheckBox>("findme").get_or(false);


	// Test the duplicate widget ragistration warning:
	cerr << "There should be a warning about duplicate widget registration below:\n";
	demo.setName("demo"); demo.setName("demo");


	//--------------------------------------------------------------------
	// Event Loop
	//--------------------------------------------------------------------
	while (window.isOpen())
	{
		demo.render();
		window.display();

		// Pass events to the GUI & check for closing or failure
		//!! Can't shortcut it when the GUI doesn't own the window, so when
		//!! it gets deactivated it wouldn't block the rest of the world, too:
		//!!while (window.pollEvent(event) && demo.process(event) || )
		while (const auto event = window.pollEvent())
		{
			demo.process(event.value());

			// Handle the window-closing explicitly, as we've configured
			// the GUI manager to not do that in this setup:
			if (event->is<sf::Event::Closed>() || // And also close on Esc:
			   (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
			{
				window.close(); // Not demo.close() if the GUI doesn't own the window!
				                // Also: window.close() will indirectly disable the GUI.
			}
		}
	}

	return EXIT_SUCCESS;

} catch(...) {
	cerr << "- ERROR: UNHANDLED EXCEPTION!" << endl;
}
