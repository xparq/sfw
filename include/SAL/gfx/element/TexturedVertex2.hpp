//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#ifndef _MCVLKUFTNHG38497YT897DNYTUINHG468R7V3D5FY_
#define _MCVLKUFTNHG38497YT897DNYTUINHG468R7V3D5FY_


#include "SAL.hpp"
#include SAL_ADAPTER(gfx/element/TexturedVertex2)

#include "SAL/gfx/element/Texture.hpp"
#include "SAL/gfx/Render.hpp"


namespace SAL::gfx
{
	class TexturedVertex2 : public TexturedVertex2_Impl
	{                 using Impl = TexturedVertex2_Impl;
	public:
		using Impl::Impl;

	//!! Will need to be more sophisticated than this! :)
	static void draw_strip(const gfx::RenderContext& ctx,
	                       const gfx::Texture& texture,
	                       const TexturedVertex2* v_array,
	                       unsigned v_count)
		{ _draw_strip(ctx, texture, v_array, v_count); }

	};
}


#endif // _MCVLKUFTNHG38497YT897DNYTUINHG468R7V3D5FY_
