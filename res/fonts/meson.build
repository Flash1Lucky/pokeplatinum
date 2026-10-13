nfgr_gen = generator(nitrogfx_exe,
    arguments: [ '@INPUT@', '@OUTPUT@', '-metadata', '@CURRENT_SOURCE_DIR@/@BASENAME@.json', ],
    output: '@BASENAME@.NFGR',
)

# Meson does not allow us to use a generator for a target like this, where we
# want to name the output quite differently from the input file. So, we use a
# custom target, then copy it into the private directory for packing.
font_base_nclr = custom_target('font.NCLR',
    output: 'font.NCLR',
    input: files('font_special_chars.png'),
    command: [ nitrogfx_exe, '@INPUT@', '@OUTPUT@', '-bitdepth', '4', '-nopad', '-pcmp' ],
)

# Editable PNG/JSON are the sources of truth; never regenerate them in a build.
family_nfgr = []
foreach font_family : ['narrow', 'narrower', 'short', 'shorter']
    family_nfgr += custom_target('font_' + font_family + '.NFGR',
        output: 'font_' + font_family + '.NFGR',
        input: files('font_' + font_family + '.png', 'font_' + font_family + '.json'),
        command: [ nitrogfx_exe, '@INPUT0@', '@OUTPUT@', '-metadata', '@INPUT1@' ],
    )
endforeach

# Legacy font_compact assets/generator are retained for recovery only.
pl_font_narc = custom_target('pl_font.narc',
    output: [
        'pl_font.narc',
        'pl_font.naix',
    ],
    input: [
        nfgr_gen.process(
            files(
                'font_system.png',
                'font_message.png',
                'font_subscreen.png',
                'font_unown.png',
            )
        ),
        ncgr_lz_gen.process(
            files('font_special_chars.png'),
            extra_args: [ '-sopc', '-version101' ],
        ),
        ncgr_gen.process(
            files('screen_indicators.png'),
            extra_args: [ '-sopc', '-version101' ],
        ),
        nclr_gen.process(
            files('screen_indicators.png'),
            extra_args: [ '-bitdepth', '4' ],
        ),
        copy_gen.process(font_base_nclr),
        copy_gen.process(family_nfgr),
    ],
    command: [
        nitroarc_exe,
        '--create',
        '--index',
        '--files-from', files('pl_font.order'),
        '--file', '@OUTPUT0@',
        '@PRIVATE_DIR@',
    ],
)

arrow_cursor_embed = custom_target('arrow_cursor.4bpp.h',
    output: [
        'arrow_cursor.4bpp',
        'arrow_cursor.4bpp.h',
    ],
    input: files('arrow_cursor.png'),
    command: [ nitrogfx_exe, '@INPUT@', '@OUTPUT0@', '-embed', 'sArrowCursorBitmap' ],
)

nitrofs_files += pl_font_narc
naix_headers += pl_font_narc[1]
embed_headers += arrow_cursor_embed[1]
