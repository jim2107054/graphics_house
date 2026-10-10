import os
import sys
from PIL import Image
from reportlab.lib.pagesizes import letter, A4
from reportlab.lib import colors
from reportlab.lib.units import inch
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, Image as RLImage, KeepTogether, PageBreak, HRFlowable
)
from reportlab.pdfgen import canvas

# 1. Convert all BMPs to PNGs
bmp_files = [
    'exterior_final.bmp',
    'interior_final.bmp',
    'car_telephone_view.bmp',
    'graveyard_pumpkins_view.bmp',
    'ghost_summon_view.bmp'
]

for bmp in bmp_files:
    png = bmp.replace('.bmp', '.png')
    if os.path.exists(bmp):
        try:
            im = Image.open(bmp)
            im.save(png, 'PNG')
            print(f"Converted {bmp} -> {png}")
        except Exception as e:
            print(f"Error converting {bmp}: {e}")

# 2. Numbered Canvas for Page Numbering and Running Header/Footer
class NumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        super(NumberedCanvas, self).__init__(*args, **kwargs)
        self._saved_page_states = []

    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()

    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.draw_page_number(num_pages)
            canvas.Canvas.showPage(self)
        canvas.Canvas.save(self)

    def draw_page_number(self, page_count):
        if self._pageNumber > 1:
            self.saveState()
            self.setFont("Helvetica", 8)
            self.setFillColor(colors.HexColor("#666666"))
            # Header
            self.drawString(54, 750, "3D Horror House at Night — Comprehensive Technical Project & Viva Report")
            self.setStrokeColor(colors.HexColor("#cccccc"))
            self.setLineWidth(0.5)
            self.line(54, 744, 558, 744)
            # Footer
            self.line(54, 45, 558, 45)
            self.drawString(54, 32, "MD Jahid Hasan Jim (Roll: 2107054) | Dept. of CSE | OpenGL Graphics Project")
            self.drawRightString(558, 32, f"Page {self._pageNumber} of {page_count}")
            self.restoreState()

def build_pdf(filename="Horror_House_Project_Report_Jim_2107054.pdf"):
    doc = SimpleDocTemplate(
        filename,
        pagesize=letter,
        leftMargin=44,
        rightMargin=44,
        topMargin=54,
        bottomMargin=54
    )

    styles = getSampleStyleSheet()

    # Custom styles
    title_style = ParagraphStyle(
        'CoverTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=22,
        leading=26,
        textColor=colors.HexColor("#1a1a2e"),
        alignment=1
    )
    subtitle_style = ParagraphStyle(
        'CoverSubtitle',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=12,
        leading=16,
        textColor=colors.HexColor("#0f3460"),
        alignment=1
    )
    meta_style = ParagraphStyle(
        'CoverMeta',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=10,
        leading=14,
        textColor=colors.HexColor("#333333"),
        alignment=1
    )
    h1_style = ParagraphStyle(
        'Heading1_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=14,
        leading=18,
        textColor=colors.HexColor("#16213e"),
        spaceBefore=14,
        spaceAfter=6
    )
    h2_style = ParagraphStyle(
        'Heading2_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=11,
        leading=15,
        textColor=colors.HexColor("#0f3460"),
        spaceBefore=10,
        spaceAfter=4
    )
    body_style = ParagraphStyle(
        'Body_Custom',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=9,
        leading=12.5,
        textColor=colors.HexColor("#222222")
    )
    code_style = ParagraphStyle(
        'Code_Custom',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=7.5,
        leading=9.5,
        textColor=colors.HexColor("#800020")
    )
    table_cell = ParagraphStyle(
        'TableCell',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8,
        leading=10,
        textColor=colors.HexColor("#111111")
    )
    table_cell_bold = ParagraphStyle(
        'TableCellBold',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=8,
        leading=10,
        textColor=colors.HexColor("#ffffff")
    )

    story = []

    # COVER / HEADER
    story.append(Spacer(1, 10))
    story.append(Paragraph("<b>COMPUTER GRAPHICS LABORATORY (CSE 4-1)</b>", subtitle_style))
    story.append(Spacer(1, 4))
    story.append(Paragraph("<b>HORROR HOUSE AT NIGHT</b>", title_style))
    story.append(Spacer(1, 4))
    story.append(Paragraph("<b>3D Real-Time OpenGL Project & Comprehensive Viva Defense Manual</b>", subtitle_style))
    story.append(Spacer(1, 10))
    story.append(HRFlowable(width="100%", thickness=1.5, color=colors.HexColor("#1a1a2e"), spaceAfter=10))

    # Meta Table
    meta_data = [
        [
            Paragraph("<b>Developer:</b> MD Jahid Hasan Jim", table_cell),
            Paragraph("<b>Roll / ID:</b> 2107054", table_cell),
            Paragraph("<b>Environment:</b> C++17, OpenGL, FreeGLUT", table_cell)
        ],
        [
            Paragraph("<b>Course:</b> Computer Graphics (4-1)", table_cell),
            Paragraph("<b>Audio:</b> WinMM Procedural Synthesis", table_cell),
            Paragraph("<b>Binary:</b> main.exe", table_cell)
        ]
    ]
    t_meta = Table(meta_data, colWidths=[180, 150, 194])
    t_meta.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#f0f4f8")),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor("#c0d0e0")),
        ('INNERGRID', (0,0), (-1,-1), 0.5, colors.HexColor("#d8e4f0")),
        ('TOPPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_meta)
    story.append(Spacer(1, 12))

    # SECTION 1: EXECUTIVE PROJECT ARCHITECTURE
    story.append(Paragraph("1. Executive Architecture & Coordinate Systems", h1_style))
    arch_p = (
        "This project is a 3D real-time graphical environment built using <b>C++17 and the OpenGL Fixed-Function Pipeline</b> "
        "with FreeGLUT for windowing/input management and WinMM for procedural zero-dependency audio synthesis. "
        "The scene employs a right-handed Cartesian coordinate system where <b>+X</b> is rightward, <b>+Y</b> is upward, "
        "and <b>+Z</b> is forward/backward. The entire haunted house structure is anchored at world origin "
        "<b>(X = -2.80m, Y = 0.0m, Z = -11.50m)</b> with a base yaw rotation of <b>-18.0&deg;</b> around the Y-axis. "
        "Every sub-object inside or attached to the house is organized within a hierarchical matrix stack (<font name='Courier'>glPushMatrix / glPopMatrix</font>), "
        "allowing modular manipulation and localized collision detection."
    )
    story.append(Paragraph(arch_p, body_style))
    story.append(Spacer(1, 8))

    # EMBEDDED SCREENSHOT 1: Exterior
    if os.path.exists('exterior_final.png'):
        story.append(KeepTogether([
            RLImage('exterior_final.png', width=500, height=220),
            Paragraph("<i>Figure 1: Full midnight exterior perspective showing the 4-light illumination, moon, cracked path, and pumpkin lanterns.</i>", table_cell),
            Spacer(1, 8)
        ]))

    # SECTION 2: LIGHTING SYSTEM ARCHITECTURE
    story.append(Paragraph("2. 4-Light Model & Illumination Mathematics", h1_style))
    light_desc = (
        "The engine features four distinct dynamic OpenGL light sources along with localized emissive pumpkin candles:<br/>"
        "&bull; <b>GL_LIGHT0 (Point Light - Porch Bulb):</b> Located at porch coordinates <font name='Courier'>(-3.37, 2.76, 3.42)</font>. Features warm golden color (1.0, 0.72, 0.28) with dynamic pendulum swaying physics.<br/>"
        "&bull; <b>GL_LIGHT1 (Directional Light - Moonlight):</b> Direction vector <font name='Courier'>(0.28, 0.85, 0.44)</font> with eerie pale-cyan diffuse tint (0.16, 0.22, 0.35).<br/>"
        "&bull; <b>GL_LIGHT2 (Spotlight - Flashlight):</b> Attached directly to first-person camera position <font name='Courier'>(g_cam.x, g_cam.y, g_cam.z)</font>, pointing along camera view vector with cutoff angle of 24&deg; and exponent 18.0.<br/>"
        "&bull; <b>GL_LIGHT3 (Area Light - Window Interior Glow):</b> Simulates interior amber light streaming through windows with diffuse tint (0.45, 0.30, 0.08)."
    )
    story.append(Paragraph(light_desc, body_style))
    story.append(Spacer(1, 10))

    # SECTION 3: OBJECT BREAKDOWN & VIVA DEFENSE GUIDE
    story.append(Paragraph("3. Detailed Object Hierarchy, Coordinates & Transformation Cheat-Sheet", h1_style))
    obj_intro = (
        "During defense and viva, examiners commonly question how each geometric object was constructed from basic primitives "
        "(Unit Cube <font name='Courier'>drawBox</font>, Cylinder <font name='Courier'>drawCylinder</font>, Sphere <font name='Courier'>drawSphere</font>, Prism <font name='Courier'>drawPrismRoof</font>), "
        "its precise spatial coordinates, transformation sequence, and how to change its properties on the spot. "
        "The table below details every single element:"
    )
    story.append(Paragraph(obj_intro, body_style))
    story.append(Spacer(1, 8))

    # Table of Objects
    headers = [
        Paragraph("<b>Object Name</b>", table_cell_bold),
        Paragraph("<b>Base Primitive &amp; Transformed Dims</b>", table_cell_bold),
        Paragraph("<b>World / Local Coords</b>", table_cell_bold),
        Paragraph("<b>Transform Chain &amp; Function</b>", table_cell_bold),
        Paragraph("<b>Dynamic? / Viva Live Edit Tip</b>", table_cell_bold)
    ]

    objects_data = [
        headers,
        [
            Paragraph("<b>Main House Core (Left Wing)</b>", table_cell),
            Paragraph("Unit Cube &rarr; 6.2m(W) &times; 4.8m(H) &times; 8.2m(D)", table_cell),
            Paragraph("Local: (-6.0, 2.4, 0.0)<br/>World: (-8.8, 2.4, -11.5)", table_cell),
            Paragraph("<font name='Courier'>drawHouseWing(-6.0, 0, 0, 6.2, 4.8, 8.2)</font><br/><i>src/main.cpp:2840</i>", table_cell),
            Paragraph("Static. Change W/H/D in <font name='Courier'>drawHouse()</font> to resize wing instantly.", table_cell)
        ],
        [
            Paragraph("<b>Central Tower &amp; Spire</b>", table_cell),
            Paragraph("Cube + 4-sided Pyramid Spire (8.8m H, 3.8m W)", table_cell),
            Paragraph("Local: (-1.8, 4.4, 0.5)<br/>World: (-4.6, 4.4, -11.0)", table_cell),
            Paragraph("<font name='Courier'>drawBox() + drawPrismRoof()</font><br/><i>src/main.cpp:2880</i>", table_cell),
            Paragraph("Static. Modify spire height parameter in <font name='Courier'>drawPrismRoof</font> to alter Gothic pitch.", table_cell)
        ],
        [
            Paragraph("<b>Side &amp; Back Windows (12 Total)</b>", table_cell),
            Paragraph("Recessed Quad + Frame + Glass Box + Crossbars", table_cell),
            Paragraph("Left: X=-9.0m, RotY=-90&deg;<br/>Right: X=6.05m, RotY=90&deg;<br/>Back: Z=-3.85m, RotY=180&deg;", table_cell),
            Paragraph("<font name='Courier'>drawHouseWindow(x, y, z, w, h, rotY)</font><br/><i>src/main.cpp:2780</i>", table_cell),
            Paragraph("Anti-glitch (0.02m offset). Rotate with <font name='Courier'>rotY</font> to snap to any wall orientation.", table_cell)
        ],
        [
            Paragraph("<b>Front Entrance Doorway</b>", table_cell),
            Paragraph("Hollow Arched Frame &amp; Recessed Plank Door", table_cell),
            Paragraph("Local: (-4.6, 1.2, 4.12)<br/>World: Door opening zone", table_cell),
            Paragraph("<font name='Courier'>drawDoorway(-4.6, 0.0, 4.12, 1.6, 2.4)</font><br/><i>src/main.cpp:2950</i>", table_cell),
            Paragraph("Only accessible entry. Collision box leaves door portal clear (<font name='Courier'>isHouseLocationFree</font>).", table_cell)
        ],
        [
            Paragraph("<b>Swinging Porch Bulb</b>", table_cell),
            Paragraph("Cylinder Wire + Socket + Glass Sphere (0.09m)", table_cell),
            Paragraph("World: (-3.37, 2.76, 3.42)<br/>Under front porch ceiling", table_cell),
            Paragraph("<font name='Courier'>glRotatef(pendulumAngle, 1, 0, 0)</font><br/><i>src/main.cpp:3020</i>", table_cell),
            Paragraph("<b>Dynamic Physics!</b> Harmonic motion: &theta;(t) = &theta;<sub>0</sub> sin(&omega;t). Toggle with <b>[B]</b>.", table_cell)
        ],
        [
            Paragraph("<b>Abandoned Vintage Car</b>", table_cell),
            Paragraph("Compound: Chasis Box + Cab Box + 4 Cylinder Wheels + Shattered Glass", table_cell),
            Paragraph("World: (7.4, 0.45, -5.2)<br/>Yaw: -32.0&deg;, Tilt: 3.5&deg;", table_cell),
            Paragraph("<font name='Courier'>drawHauntedCar(7.4, -5.2, -32.0)</font><br/><i>src/main.cpp:2500</i>", table_cell),
            Paragraph("Static. Change X, Z in <font name='Courier'>drawSceneObjects()</font> to reposition anywhere on terrain.", table_cell)
        ],
        [
            Paragraph("<b>Leaning Telephone Pole</b>", table_cell),
            Paragraph("Tapered Cylinder (8.5m H) + 2 Crossarm Boxes + Glass Insulators", table_cell),
            Paragraph("World: (9.2, 0.0, -4.5)<br/>Tilt: 6.5&deg; into soil", table_cell),
            Paragraph("<font name='Courier'>drawTelephonePole(9.2, -4.5, 6.5)</font><br/><i>src/main.cpp:2580</i>", table_cell),
            Paragraph("Static. Change <font name='Courier'>leanAngle</font> argument to tilt pole dynamically.", table_cell)
        ],
        [
            Paragraph("<b>Jack-o'-Lantern Pumpkins (14 Total)</b>", table_cell),
            Paragraph("Segmented Ribbed Sphere (12 latitudinal wedges) + Stem + Carved Triangles", table_cell),
            Paragraph("Flanking stone path from Z=-8.5m to Z=14.0m", table_cell),
            Paragraph("<font name='Courier'>drawCarvedPumpkin(x, z, size, rotY, true)</font><br/><i>src/main.cpp:2150</i>", table_cell),
            Paragraph("<b>Dynamic Flicker!</b> Flame light fluctuates with noise function. Toggle with <b>[5/K]</b>.", table_cell)
        ],
        [
            Paragraph("<b>Dilapidated Interior Table &amp; Chairs</b>", table_cell),
            Paragraph("Split Box Tabletop propped on Brick + Broken Leg Spindles", table_cell),
            Paragraph("Interior: (-7.5, 0.1, -9.2)<br/>Inside main living room", table_cell),
            Paragraph("<font name='Courier'>drawDilapidatedInterior()</font><br/><i>src/main.cpp:3200</i>", table_cell),
            Paragraph("Static ruined props. Shows broken joints, collapsed boards, fallen antique books.", table_cell)
        ],
        [
            Paragraph("<b>Ghost Apparition (Summonable)</b>", table_cell),
            Paragraph("Deformed Tapered Sphere Sheet + Hollow Black Eye Sockets", table_cell),
            Paragraph("Spawns at (-4.6, 1.45, -3.2)<br/>Swoops outward to (+1.5m)", table_cell),
            Paragraph("<font name='Courier'>drawGhostApparition()</font><br/><i>src/main.cpp:3320</i>", table_cell),
            Paragraph("<b>Dynamic Entity!</b> Press <b>[E] / [J]</b> to summon with sound synthesis &amp; alpha fade.", table_cell)
        ]
    ]

    t_objs = Table(objects_data, colWidths=[95, 105, 95, 110, 115])
    t_objs.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#16213e")),
        ('ALIGN', (0,0), (-1,-1), 'LEFT'),
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#b0c4de")),
        ('ROWBACKGROUNDS', (0,1), (-1,-1), [colors.HexColor("#ffffff"), colors.HexColor("#f8fafc")]),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_objs)
    story.append(Spacer(1, 14))

    # EMBEDDED SCREENSHOT 2 & 3: Interior & Ghost
    img_grid = []
    if os.path.exists('interior_final.png') and os.path.exists('ghost_summon_view.png'):
        img_grid.append([
            RLImage('interior_final.png', width=245, height=140),
            RLImage('ghost_summon_view.png', width=245, height=140)
        ])
        img_grid.append([
            Paragraph("<i>Figure 2: Dilapidated haunted interior with collapsed table, ruined rafters, and cobwebs.</i>", table_cell),
            Paragraph("<i>Figure 3: Summoned Ghost Apparition swooping near the porch doorway with chilling audio.</i>", table_cell)
        ])
        t_imgs = Table(img_grid, colWidths=[255, 255])
        t_imgs.setStyle(TableStyle([
            ('ALIGN', (0,0), (-1,-1), 'CENTER'),
            ('VALIGN', (0,0), (-1,-1), 'TOP'),
            ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ]))
        story.append(KeepTogether(t_imgs))
        story.append(Spacer(1, 10))

    # SECTION 4: DYNAMIC SIMULATION & PHYSICS
    story.append(Paragraph("4. Real-Time Dynamic Simulations & Mathematical Modeling", h1_style))
    sim_desc = (
        "<b>1. Pendulum Swiveling Porch Lamp:</b> Uses 2nd-order damped harmonic oscillation equation "
        "<font name='Courier'>&theta;(t) = &theta;<sub>max</sub> &times; sin(2.2 &times; t)</font> driving rotational transform matrix.<br/>"
        "<b>2. Procedural Falling Leaves System:</b> 45 airborne particles with 6-DOF simulation (position drift <font name='Courier'>v<sub>x</sub>, v<sub>y</sub>, v<sub>z</sub></font> "
        "and 3D tumbling rates <font name='Courier'>&omega;<sub>x</sub>, &omega;<sub>y</sub>, &omega;<sub>z</sub></font>) respawning in upper tree canopy.<br/>"
        "<b>3. Procedural Audio Synthesis:</b> Zero-dependency PCM waveform generation producing 44.1kHz 16-bit audio headers in-memory for ambient night howling wind, thunderclaps, and ghost wails.<br/>"
        "<b>4. Door-Only Entry Wall Collision Engine:</b> Axis-aligned local boundary tester (<font name='Courier'>isHouseLocationFree(x, z)</font>) "
        "that blocks camera penetration through all exterior siding while permitting doorway navigation with sliding friction."
    )
    story.append(Paragraph(sim_desc, body_style))
    story.append(Spacer(1, 10))

    # EMBEDDED SCREENSHOT 4 & 5: Car & Graveyard
    img_grid2 = []
    if os.path.exists('car_telephone_view.png') and os.path.exists('graveyard_pumpkins_view.png'):
        img_grid2.append([
            RLImage('car_telephone_view.png', width=245, height=135),
            RLImage('graveyard_pumpkins_view.png', width=245, height=135)
        ])
        img_grid2.append([
            Paragraph("<i>Figure 4: Abandoned vintage automobile, rusted chassis, shattered windshield, and leaning pole.</i>", table_cell),
            Paragraph("<i>Figure 5: Graveyard cemetery with tombstones, leaning wooden crosses, and glowing pumpkin trail.</i>", table_cell)
        ])
        t_imgs2 = Table(img_grid2, colWidths=[255, 255])
        t_imgs2.setStyle(TableStyle([
            ('ALIGN', (0,0), (-1,-1), 'CENTER'),
            ('VALIGN', (0,0), (-1,-1), 'TOP'),
            ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ]))
        story.append(KeepTogether(t_imgs2))
        story.append(Spacer(1, 10))

    # SECTION 5: INTERACTIVE KEYBOARD & VIVA CONTROLS
    story.append(Paragraph("5. Interactive Controls & Examiner Demonstration Guide", h1_style))
    ctrl_data = [
        [Paragraph("<b>Key Command</b>", table_cell_bold), Paragraph("<b>Function / Demonstrated Feature</b>", table_cell_bold), Paragraph("<b>Code Reference</b>", table_cell_bold)],
        [Paragraph("<b>[E]</b> or <b>[J]</b>", table_cell), Paragraph("<b>Summon Spooky Ghost Apparition</b> with procedural sound effect", table_cell), Paragraph("<font name='Courier'>triggerGhostSummon()</font>", table_cell)],
        [Paragraph("<b>[1] [2] [3/F] [4] [5/K] [0]</b>", table_cell), Paragraph("Toggle Point, Moon, Spotlight, Area, Candle & Master Lights", table_cell), Paragraph("<font name='Courier'>g_light0PointOn ...</font>", table_cell)],
        [Paragraph("<b>[W] [A] [S] [D] + Mouse</b>", table_cell), Paragraph("First-person walk navigation with door-only wall collision", table_cell), Paragraph("<font name='Courier'>processKeyboardInput()</font>", table_cell)],
        [Paragraph("<b>[B]</b>", table_cell), Paragraph("Toggle Porch Bulb Harmonic Sway &amp; Voltage Flicker", table_cell), Paragraph("<font name='Courier'>g_bulbSwayEnabled</font>", table_cell)],
        [Paragraph("<b>[C]</b>", table_cell), Paragraph("Toggle Cinematic Auto-Tour Presentation Camera Spline", table_cell), Paragraph("<font name='Courier'>g_cinematicMode</font>", table_cell)],
        [Paragraph("<b>[L]</b>", table_cell), Paragraph("Trigger Manual Lightning Bolt Strike with Sky Flash", table_cell), Paragraph("<font name='Courier'>triggerLightning()</font>", table_cell)],
        [Paragraph("<b>[T] / [G] / [M]</b>", table_cell), Paragraph("Toggle Texture Mapping / Atmospheric Fog / Ambient Audio", table_cell), Paragraph("<font name='Courier'>g_texEnabled / g_fogEnabled</font>", table_cell)],
        [Paragraph("<b>[P] / [R]</b>", table_cell), Paragraph("Capture High-Res Screenshot (.bmp) / Reset Camera Vantage Point", table_cell), Paragraph("<font name='Courier'>saveScreenshot()</font>", table_cell)]
    ]
    t_ctrl = Table(ctrl_data, colWidths=[120, 240, 160])
    t_ctrl.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#0f3460")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#c0d0e0")),
        ('ROWBACKGROUNDS', (0,1), (-1,-1), [colors.HexColor("#ffffff"), colors.HexColor("#f0f4f8")]),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_ctrl)
    story.append(Spacer(1, 14))

    # SECTION 6: HOW TO MODIFY OBJECTS INSTANTLY DURING VIVA
    story.append(Paragraph("6. Quick Live-Code Editing Guide for Examiner Requests", h1_style))
    viva_tips = (
        "If the examiner asks you to modify any object during viva, here is exactly where to edit:<br/>"
        "&bull; <b>Move the Car:</b> In <font name='Courier'>src/main.cpp</font> (line ~7610), change <font name='Courier'>drawHauntedCar(7.4f, -5.2f, -32.0f)</font> &rarr; change 7.4f (X) or -5.2f (Z).<br/>"
        "&bull; <b>Change House Size:</b> In <font name='Courier'>drawHouse()</font> (line ~2830), change the width/height parameters of <font name='Courier'>drawHouseWing()</font>.<br/>"
        "&bull; <b>Change Window Colors/Glow:</b> In <font name='Courier'>drawHouseWindow()</font> (line ~2780), edit the yellow-orange RGB constants <font name='Courier'>glColor4f(1.0f, 0.72f, 0.25f, 1.0f)</font>.<br/>"
        "&bull; <b>Change Ghost Speed/Position:</b> In <font name='Courier'>GhostEntity g_ghost</font> (line ~530), adjust <font name='Courier'>g_ghost.x, y, z</font> and duration."
    )
    story.append(Paragraph(viva_tips, body_style))

    # Build PDF
    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"Successfully generated PDF report: {filename}")

if __name__ == '__main__':
    build_pdf()
