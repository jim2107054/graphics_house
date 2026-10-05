import os
import sys
from PIL import Image
from reportlab.lib.pagesizes import A4
from reportlab.lib import colors
from reportlab.lib.units import inch
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, Image as RLImage, KeepTogether, PageBreak, HRFlowable
)
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.pdfgen import canvas

# Register Bengali Unicode Font
font_path = 'C:/Windows/Fonts/kalpurush.ttf'
if os.path.exists(font_path):
    pdfmetrics.registerFont(TTFont('Kalpurush', font_path))
    BN_FONT = 'Kalpurush'
else:
    BN_FONT = 'Helvetica'

# Convert BMPs to PNGs
bmps = [
    'house_facade.bmp',
    'side_windows_left.bmp',
    'doorway_entry.bmp',
    'swinging_bulb.bmp',
    'haunted_car.bmp',
    'telephone_pole.bmp',
    'pumpkins_path.bmp',
    'graveyard_crosses.bmp',
    'dilapidated_interior.bmp',
    'ghost_summon.bmp',
    'moon_sky.bmp',
    'exterior_final.bmp'
]

for b in bmps:
    p = b.replace('.bmp', '.png')
    if os.path.exists(b):
        try:
            im = Image.open(b)
            im.save(p, 'PNG')
            print(f"Converted {b} -> {p}")
        except Exception as e:
            print(f"Error {b}: {e}")

class BanglaNumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        super(BanglaNumberedCanvas, self).__init__(*args, **kwargs)
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
            self.setFillColor(colors.HexColor("#555555"))
            # Header
            self.drawString(40, 800, "Horror House at Night (3D OpenGL) - Full Project & Viva Report (Bangla)")
            self.setStrokeColor(colors.HexColor("#cccccc"))
            self.setLineWidth(0.5)
            self.line(40, 794, 555, 794)
            # Footer
            self.line(40, 45, 555, 45)
            self.drawString(40, 32, "MD Jahid Hasan Jim (Roll: 2107054) | Computer Graphics (CSE 4-1)")
            self.drawRightString(555, 32, f"Page {self._pageNumber} of {page_count}")
            self.restoreState()

def build_bangla_pdf(filename="Horror_House_Full_Viva_Report_Bangla.pdf"):
    doc = SimpleDocTemplate(
        filename,
        pagesize=A4,
        leftMargin=36,
        rightMargin=36,
        topMargin=48,
        bottomMargin=48
    )

    styles = getSampleStyleSheet()

    # Define custom styles using Kalpurush font
    title_style = ParagraphStyle(
        'BN_Title',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=20,
        leading=24,
        textColor=colors.HexColor("#111827"),
        alignment=1
    )
    subtitle_style = ParagraphStyle(
        'BN_Subtitle',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=11,
        leading=15,
        textColor=colors.HexColor("#1e3a8a"),
        alignment=1
    )
    h1_style = ParagraphStyle(
        'BN_H1',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=13,
        leading=17,
        textColor=colors.HexColor("#0f172a"),
        spaceBefore=12,
        spaceAfter=5
    )
    h2_style = ParagraphStyle(
        'BN_H2',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=10.5,
        leading=14,
        textColor=colors.HexColor("#1e40af"),
        spaceBefore=8,
        spaceAfter=3
    )
    body_style = ParagraphStyle(
        'BN_Body',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=8.5,
        leading=12,
        textColor=colors.HexColor("#1f2937")
    )
    body_bold = ParagraphStyle(
        'BN_Body_Bold',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=8.5,
        leading=12,
        textColor=colors.HexColor("#000000")
    )
    code_style = ParagraphStyle(
        'BN_Code',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=7.5,
        leading=9.5,
        textColor=colors.HexColor("#831843")
    )
    table_cell = ParagraphStyle(
        'BN_TableCell',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=7.8,
        leading=10.5,
        textColor=colors.HexColor("#111827")
    )
    table_cell_bold = ParagraphStyle(
        'BN_TableCellBold',
        parent=styles['Normal'],
        fontName=BN_FONT,
        fontSize=8,
        leading=10.5,
        textColor=colors.HexColor("#ffffff")
    )

    story = []

    # COVER HEADER
    story.append(Paragraph("<b>কম্পিউটার গ্রাফিক্স ল্যাবরেটরি প্রজেক্ট (CSE 4-1)</b>", subtitle_style))
    story.append(Spacer(1, 3))
    story.append(Paragraph("<b>হোরর হাউজ অ্যাট নাইট (3D Horror House at Night)</b>", title_style))
    story.append(Spacer(1, 3))
    story.append(Paragraph("<b>সম্পূর্ণ প্রজেক্ট ডকুমেন্টেশন, অবজেক্ট কোঅর্ডিনেশন, ফাংশন ব্যাখ্যা ও ভাইভা ডিফেন্স গাইড</b>", subtitle_style))
    story.append(Spacer(1, 6))
    story.append(HRFlowable(width="100%", thickness=1.5, color=colors.HexColor("#0f172a"), spaceAfter=8))

    # Meta Table
    meta_data = [
        [
            Paragraph("<b>ডেভেলপার:</b> MD Jahid Hasan Jim", table_cell),
            Paragraph("<b>রোল / আইডি:</b> 2107054", table_cell),
            Paragraph("<b>প্রযুক্তি:</b> C++17, OpenGL, FreeGLUT", table_cell)
        ],
        [
            Paragraph("<b>কোর্স:</b> Computer Graphics (4-1)", table_cell),
            Paragraph("<b>অডিও:</b> WinMM Procedural Waveform", table_cell),
            Paragraph("<b>সোর্স ফাইল:</b> src/main.cpp", table_cell)
        ]
    ]
    t_meta = Table(meta_data, colWidths=[175, 145, 203])
    t_meta.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#f1f5f9")),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor("#cbd5e1")),
        ('INNERGRID', (0,0), (-1,-1), 0.5, colors.HexColor("#e2e8f0")),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_meta)
    story.append(Spacer(1, 8))

    # SECTION 1: প্রজেক্ট আর্কিটেকচার ও গ্লোবাল কোঅর্ডিনেট সিস্টেম
    story.append(Paragraph("১. প্রজেক্ট আর্কিটেকচার ও গ্লোবাল কোঅর্ডিনেট সিস্টেম (Global Coordinate System)", h1_style))
    p1 = (
        "এই প্রজেক্টটি C++17 এবং ওপেনজিএল (OpenGL Fixed-Function Pipeline) ব্যবহার করে তৈরি একটি রিয়েল-টাইম থ্রিডি হরর সিন। "
        "এখানে <b>Right-Handed Cartesian Coordinate System</b> অনুসরণ করা হয়েছে, যেখানে <b>+X</b> ডানদিক, <b>+Y</b> ওপরের দিক, "
        "এবং <b>+Z</b> আমাদের চোখের দিকে বা স্ক্রিন থেকে বাইরের দিকে নির্দেশ করে। "
        "পুরো হাউজ স্ট্রাকচারটি মূল সিন অরিজিন থেকে <font name='Courier'>T(-2.80, 0.0, -11.50)</font> এবং Y-অক্ষের সাপেক্ষে "
        "<font name='Courier'>R_y(-18.0&deg;)</font> তে রোটেট করা। প্রতিটি সাব-অবজেক্ট OpenGL ম্যাট্রিক্স স্ট্যাক "
        "(<font name='Courier'>glPushMatrix / glPopMatrix</font>) এর মাধ্যমে নিজস্ব লোকাল কোঅর্ডিনেট ফ্রেমে নিখুঁতভাবে রেন্ডার হয়।"
    )
    story.append(Paragraph(p1, body_style))
    story.append(Spacer(1, 6))

    # SECTION 2: ৪-লাইট মডেল ও লাইটিং ইকুয়েশন
    story.append(Paragraph("২. ৪-লাইট মডেল ও লাইটিং সমীকরণ (Lighting Mathematics & Implementation)", h1_style))
    p_light = (
        "সিনটিতে বাস্তবসম্মত ভূতুড়ে পরিবেশ ফুটিয়ে তুলতে ৪টি ভিন্ন OpenGL লাইট ও প্রসিডিউরাল ইমিসিভ ক্যান্ডেল ব্যবহৃত হয়েছে:<br/>"
        "&bull; <b>GL_LIGHT0 (Point Light - বারান্দার ঝুলন্ত বাল্ব):</b> পজিশন <font name='Courier'>(-3.37, 2.76, 3.42)</font>। ওয়ার্ম গোল্ডেন আলো (1.0, 0.72, 0.28)। এটি হারমোনিক মোশনে দোলে এবং আলোর তীব্রতা ভোল্টেজ ফ্লিকারের মতো ওঠানামা করে।<br/>"
        "&bull; <b>GL_LIGHT1 (Directional Light - চাঁদের আলো Moonlight):</b> দিক ভেক্টর <font name='Courier'>(0.28, 0.85, 0.44)</font>। সায়ান-ব্লু টিল কালার (0.16, 0.22, 0.35)।<br/>"
        "&bull; <b>GL_LIGHT2 (Spotlight - প্লেয়ারের টর্চলাইট Flashlight):</b> ক্যামেরার সাথে সংযুক্ত। ক্যামেরার ডিরেকশন বরাবর আলো ফেলে (<font name='Courier'>Cutoff = 24&deg;, Exponent = 18.0</font>)।<br/>"
        "&bull; <b>GL_LIGHT3 (Area Light - জানালার ভেতরের আভা Window Interior Glow):</b> ঘরের ভেতর থেকে আসা অ্যাম্বার আলো (0.45, 0.30, 0.08)।<br/>"
        "&bull; <b>Jack-o'-Lantern Candles (ইমিসিভ ক্যান্ডেল আলো):</b> রাস্তার পাশের ১৪টি কুমড়ার ভেতরে আগুনের শিখা নয়েজ ফাংশন দিয়ে ওঠানামা করে।"
    )
    story.append(Paragraph(p_light, body_style))
    story.append(Spacer(1, 8))

    # SECTION 3: প্রতিটি অবজেক্টের বিস্তারিত টেকনিক্যাল বিশ্লেষণ ও স্ক্রিনশট (Object-by-Object Breakdown)
    story.append(Paragraph("৩. প্রতিটি অবজেক্টের বিস্তারিত টেকনিক্যাল ব্রেকডাউন (Object Breakdown with Screenshots)", h1_style))
    story.append(Paragraph("নিচে প্রতিটি অবজেক্টের ছবি, ইনিশিয়াল বেসিক প্রিমিটিভ, কীভাবে ট্রান্সফর্মেশন করে বানানো হয়েছে, কোঅর্ডিনেট, ফাংশন এবং ইনস্ট্যান্ট এডিট করার উপায় দেওয়া হলো:", body_style))
    story.append(Spacer(1, 6))

    # Helper function to generate an object section with image and text
    def add_object_detail(title, img_name, base_prim, coords, transform_flow, func_name, live_edit_tip, math_eq=""):
        items = []
        items.append(Paragraph(f"<b>{title}</b>", h2_style))
        
        # Two column layout: Left Image, Right Details Table
        col1_w = 200
        col2_w = 315
        
        img_flow = None
        if os.path.exists(img_name):
            img_flow = RLImage(img_name, width=190, height=105)
        else:
            img_flow = Paragraph("<i>Image not found</i>", table_cell)

        desc_content = [
            Paragraph(f"<b>ইনিশিয়াল প্রিমিটিভ (Base Primitive):</b> {base_prim}", table_cell),
            Paragraph(f"<b>কোঅর্ডিনেট (Coordinates):</b> {coords}", table_cell),
            Paragraph(f"<b>ফাংশন (Code Reference):</b> <font name='Courier'>{func_name}</font>", table_cell),
            Paragraph(f"<b>ট্রান্সফর্মেশন ধাপ:</b> {transform_flow}", table_cell),
            Paragraph(f"<b>ইনস্ট্যান্ট এডিট করার উপায় (Live Viva Tip):</b> {live_edit_tip}", table_cell)
        ]
        if math_eq:
            desc_content.append(Paragraph(f"<b>ফিজিক্স / গাণিতিক সূত্র:</b> {math_eq}", table_cell))

        t_obj = Table([[img_flow, desc_content]], colWidths=[col1_w, col2_w])
        t_obj.setStyle(TableStyle([
            ('VALIGN', (0,0), (-1,-1), 'TOP'),
            ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#f8fafc")),
            ('BOX', (0,0), (-1,-1), 0.5, colors.HexColor("#cbd5e1")),
            ('TOPPADDING', (0,0), (-1,-1), 4),
            ('BOTTOMPADDING', (0,0), (-1,-1), 4),
            ('LEFTPADDING', (0,0), (-1,-1), 4),
            ('RIGHTPADDING', (0,0), (-1,-1), 4),
        ]))
        items.append(t_obj)
        items.append(Spacer(1, 6))
        return KeepTogether(items)

    # 1. Main Haunted House Facade
    story.append(add_object_detail(
        "১. মূল ভিক্টোরিয়ান হরর বাড়ি (Main Victorian House Facade)",
        "house_facade.png",
        "Unit Cube (drawBox) + 4-sided Pyramid (drawPrismRoof) + Cylinder Columns",
        "Local: (-2.80m, 0.0m, -11.50m), Base Rotation: RotY = -18.0&deg;",
        "glPushMatrix &rarr; glTranslatef &rarr; glRotatef(-18) &rarr; ৩টি উইং (Left, Center Tower, Right) কম্বাইন করা হয়েছে।",
        "drawHouse(), drawHouseWing() (src/main.cpp:2840)",
        "drawHouseWing() এর W/H/D প্যারামিটার চেঞ্জ করলে বাড়ির আকার ছোট/বড় হবে।"
    ))

    # 2. Side & Back Windows
    story.append(add_object_detail(
        "২. দেয়ালের জানালাসমূহ (Side & Back Windows - ১২টি মোট)",
        "side_windows_left.png",
        "Recessed Quad + Wood Frame Box + Glowing Glass Quad + Crossbars",
        "Left: X=-9.00m, RotY=-90&deg; | Right: X=6.05m, RotY=90&deg; | Back: Z=-3.85m, RotY=180&deg;",
        "glTranslatef(x, y, z) &rarr; glRotatef(rotY, 0, 1, 0) &rarr; Frame (0.02m offset) &rarr; Core (0.006m offset)",
        "drawHouseWindow() (src/main.cpp:2780)",
        "rotY দিয়ে জানালা যেকোনো দেয়ালে বসানো যায়। অফসেট থাকায় কোনো Z-fighting গ্লিচ হয় না।"
    ))

    # 3. Front Doorway & Entrance
    story.append(add_object_detail(
        "৩. সামনের মূল দরজা (Front Doorway & Porch Portal)",
        "doorway_entry.png",
        "Archway Outer Box Frame + Recessed Dark Wooden Door Plank",
        "Local: X=-4.6m, Y=1.2m, Z=4.12m (Door Span: X &isin; [-5.40, -3.80])",
        "glTranslatef &rarr; drawBox(Outer Frame) &rarr; glTranslatef(Recess) &rarr; drawBox(Door)",
        "drawDoorway() (src/main.cpp:2950)",
        "isHouseLocationFree() এ এই পোর্টালটি ক্লিয়ার রাখা হয়েছে, ফলে শুধু এই দরজা দিয়েই ঘরে ঢোকা যায়।"
    ))

    # 4. Swinging Porch Bulb
    story.append(add_object_detail(
        "৪. বারান্দার ঝুলন্ত হারমোনিক বাল্ব (Swinging Porch Light)",
        "swinging_bulb.png",
        "Cylinder Wire (0.65m) + Socket Box + Translucent Glass Sphere (0.09m)",
        "World: (-3.37m, 2.76m, 3.42m) under porch ceiling",
        "glPushMatrix &rarr; glTranslatef(Pivot) &rarr; glRotatef(theta, 1, 0, 0) &rarr; drawCylinder &rarr; drawSphere",
        "drawSwingingBulb() (src/main.cpp:3020)",
        "কীবোর্ডের [B] চাপলে দুলন ও ভোল্টেজ ফ্লিকার টগল হবে।",
        "&theta;(t) = &theta;<sub>amp</sub> &times; sin(2.2 &times; t), Light0_pos = Pivot + R(&theta;) &times; L"
    ))

    # 5. Abandoned Vintage Car
    story.append(add_object_detail(
        "৫. পরিত্যক্ত ভাঙা পুরনো গাড়ি (Abandoned Vintage Automobile)",
        "haunted_car.png",
        "Chassis Box (4.4x0.6x2.1m) + Cabin Box + 4 Cylinder Wheels + Shattered Glass Line Strip",
        "World: (X=7.4m, Z=-5.2m), Yaw=-32.0&deg;, Soil Tilt=3.5&deg;",
        "glPushMatrix &rarr; glTranslatef(7.4, gy, -5.2) &rarr; glRotatef(-32, 0, 1, 0) &rarr; glRotatef(3.5, 0, 0, 1) &rarr; Parts",
        "drawHauntedCar() (src/main.cpp:2500)",
        "src/main.cpp line ~7610 এ drawHauntedCar(7.4f, -5.2f, -32.0f) এর X, Z বা Yaw কোণ পরিবর্তন করলেই গাড়ি যেকোনো জায়গায় সরে যাবে।"
    ))

    # 6. Leaning Telephone Pole
    story.append(add_object_detail(
        "৬. হেলে পড়া কাঠের বৈদ্যুতিক খুঁটি (Leaning Historic Telephone Pole)",
        "telephone_pole.png",
        "Tapered Cylinder (8.5m H, R_base=0.14m) + 2 Crossarm Boxes + 4 Cylinder Insulators + Line Strips",
        "World: (X=9.2m, Z=-4.5m), Soil Lean = 6.5&deg;",
        "glTranslatef(9.2, gy, -4.5) &rarr; glRotatef(6.5, 0, 0, 1) &rarr; drawCylinder(Trunk) &rarr; drawBox(Arms)",
        "drawTelephonePole() (src/main.cpp:2580)",
        "leanAngle আর্গুমেন্ট চেঞ্জ করে খুঁটিকে মাটির দিকে আরও বেশি বা কম কাত করা যায়।"
    ))

    # 7. Jack-o'-Lantern Pumpkins
    story.append(add_object_detail(
        "৭. ভূতুড়ে আলো জ্বলন্ত কুমড়ো (Jack-o'-Lantern Pumpkins - ১৪টি)",
        "pumpkins_path.png",
        "১২টি ল্যাটিটিউডিনাল রিবড স্লাইস (drawSphere) + Curved Cylinder বোঁটা + ত্রিকোনাকার চোখ-মুখ কাটার ভার্টেক্স",
        "পাথরের রাস্তার দুই পাশ বরাবর (Z = -8.5m থেকে Z = 14.0m পর্যন্ত ছড়ানো)",
        "glTranslatef(x, y, z) &rarr; glRotatef(rotY) &rarr; drawSphere(Ribs) &rarr; glColor3f(Fire) &rarr; GL_TRIANGLES(Mouth)",
        "drawCarvedPumpkin() (src/main.cpp:2150)",
        "কীবোর্ডের [5] বা [K] চাপলে মোমবাতি অন/অফ হবে।",
        "I<sub>flame</sub>(t) = 0.85 + 0.15 &times; sin(14.0 &times; t) &times; cos(9.3 &times; t)"
    ))

    # 8. Dilapidated Interior Furniture
    story.append(add_object_detail(
        "৮. ঘরের ভেতরের ভাঙাচোরা আসবাবপত্র (Dilapidated Ruined Interior)",
        "dilapidated_interior.png",
        "মাঝখান থেকে ভাঙা টেবিল বক্স + ইটের ঠেস + এক পায়া ছোট ভাঙা চেয়ার + মেঝের ফাঁকা তক্তা + মাকড়সার জাল",
        "Interior Coordinates: (-7.50m, 0.10m, -9.20m) ঘরের ভেতরের লিভিং রুম",
        "glTranslatef &rarr; টেবিলের দুই টুকরাকে আলাদা কৌণিক কাত (tilt) করে বসানো &rarr; ভাঙা পায়া ও ছিটকে পড়া তক্তা রেন্ডার",
        "drawDilapidatedInterior() (src/main.cpp:3200)",
        "ভেতরে যাওয়ার জন্য সামনের দরজা দিয়ে হেঁটে ঢুকতে হবে। [W][A][S][D] দিয়ে ঘুরে ঘুরে পুরো ধ্বংসপ্রাপ্ত ঘর দেখা যাবে।"
    ))

    # 9. Summonable Ghost Apparition
    story.append(add_object_detail(
        "৯. শূন্যে ভেসে আসা ভূতের ছায়া (Summonable Ghost Entity)",
        "ghost_summon.png",
        "ডিফর্মড টেপারড স্ফিয়ার শিট মেশ + ফাঁপা কালো চোখের স্ফিয়ার সকেট + আলফা ব্লেন্ডিং ট্রাঞ্জিশন",
        "Spawns at (-4.6m, 1.45m, -3.2m) &rarr; Swoops to (+1.5m) বারান্দার মুখে",
        "glEnable(GL_BLEND) &rarr; glColor4f(0.85, 0.95, 1.0, alpha) &rarr; glTranslatef &rarr; drawSphere",
        "drawGhostApparition() (src/main.cpp:3320)",
        "কীবোর্ডের [E] বা [J] বাটন চাপলে ভূত আবির্ভূত হবে এবং প্রসিডিউরাল হরর চিৎকার অডিও প্লে হবে।",
        "&alpha;(t) = e<sup>-t/3.0</sup>, Z(t) = Z<sub>start</sub> + (Z<sub>target</sub> - Z<sub>start</sub>) &times; (1 - e<sup>-1.5t</sup>)"
    ))

    # 10. Moon, Clouds & Sky
    story.append(add_object_detail(
        "১০. বিশাল পূর্ণিমা চাঁদ ও নৈশ আকাশ (Giant Moon, Fog & Night Sky)",
        "moon_sky.png",
        "Textured Moon Billboard Quad (14.0m ব্যাস) + Gradient Sky Dome + Multi-octave Procedural Cloud Planes",
        "World: Moon at (0.0m, 32.0m, -48.0m)",
        "glPushMatrix &rarr; glTranslatef(MoonPos) &rarr; bindTexture(TEX_MOON) &rarr; glBegin(GL_QUADS)",
        "drawSky(), drawMoon() (src/main.cpp:1850)",
        "কীবোর্ডের [G] চাপলে দূরবর্তী কুয়াশা (Fog) অন/অফ হবে।"
    ))

    # SECTION 4: ডাইনামিক সিন ও ম্যাথমেটিক্যাল সিমুলেশন
    story.append(Paragraph("৪. ডাইনামিক সিন ও ম্যাথমেটিক্যাল সিমুলেশন (Dynamic Simulations & Physics)", h1_style))
    p_sim = (
        "<b>১. ঝুলন্ত বাল্বের হারমোনিক দুলন (Pendulum Physics):</b><br/>"
        "বারান্দার বাল্বটি দ্বিতীয় মাত্রার ব্যবকলনীয় সমীকরণ (2nd Order Harmonic Oscillator) মেনে দোলে: "
        "<font name='Courier'>&theta;(t) = &theta;<sub>0</sub> &times; sin(&omega; t)</font>। "
        "এখানে <font name='Courier'>&omega; = 2.2 rad/s</font>। এর ফলে বাল্বের আলো প্রতি ফ্রেমে বিশ্ব স্থানাঙ্কে নতুন পজিশনে আপডেট হয়।<br/><br/>"
        "<b>২. ৬-ডিগ্রি ফ্রিডম বাতাসের শুকনো পাতা ওড়ার ইঞ্জিন (Falling Leaves Particle Physics):</b><br/>"
        "সিনটিতে ৪৫টি স্বতন্ত্র পাতার পার্টিকেল রয়েছে। প্রতি পাতার জন্য <font name='Courier'>v<sub>x</sub>, v<sub>y</sub>, v<sub>z</sub></font> ড্রিফট গতিবেগ এবং "
        "<font name='Courier'>&omega;<sub>x</sub>, &omega;<sub>y</sub>, &omega;<sub>z</sub></font> থ্রিডি ঘূর্ণন গণনা করা হয়। "
        "পাতা মাটিতে পড়লে তা স্বয়ংক্রিয়ভাবে ডানদিকের বড় গাছের ওপরে আবার রিস্পন (Respawn) করে।<br/><br/>"
        "<b>৩. ডোর-অনলি দেয়াল কলিশন ও স্লাইডিং সিস্টেম (Wall Collision & Doorway Only Entry):</b><br/>"
        "প্লেয়ার যখন হাঁটে, তখন তার ক্যামেরা পজিশন <font name='Courier'>(x, z)</font> কে ইনভার্স রোটেশন ম্যাট্রিক্স দিয়ে বাড়ির লোকাল ফ্রেমে নেওয়া হয়:<br/>"
        "&nbsp;&nbsp;&nbsp;&nbsp;<font name='Courier'>x_loc = (x - x_orig) * cos(18&deg;) - (z - z_orig) * sin(18&deg;)</font><br/>"
        "&nbsp;&nbsp;&nbsp;&nbsp;<font name='Courier'>z_loc = (x - x_orig) * sin(18&deg;) + (z - z_orig) * cos(18&deg;)</font><br/>"
        "এরপর <font name='Courier'>isHouseLocationFree()</font> চেক করে যে প্লেয়ার কোনো দেয়ালে ধাক্কা খাচ্ছে কিনা। "
        "কেবলমাত্র সামনের মূল দরজা (<font name='Courier'>x_loc &isin; [-5.4, -3.8], z_loc &approx; 4.78</font>) উন্মুক্ত থাকায় শুধু দরজা দিয়েই ঘরে ঢোকা সম্ভব।"
    )
    story.append(Paragraph(p_sim, body_style))
    story.append(Spacer(1, 8))

    # SECTION 5: ফাংশন বাই ফাংশন কোড ব্যাখ্যা (Code Line-by-Line Breakdown)
    story.append(Paragraph("৫. গুরুত্বপূর্ণ ফাংশনসমূহের লাইন-বাই-লাইন বিশ্লেষণ (Key Functions Breakdown)", h1_style))
    
    func_data = [
        [Paragraph("<b>ফাংশনের নাম</b>", table_cell_bold), Paragraph("<b>কী কাজ করে ও লাইনের কার্যপদ্ধতি</b>", table_cell_bold), Paragraph("<b>মূল কোড টেকনিক</b>", table_cell_bold)],
        [
            Paragraph("<b>drawHouseWindow()</b><br/><i>Line: 2780</i>", table_cell),
            Paragraph("দেয়ালের জানালার ফ্রেম, ভেতরের গ্লাস ও ক্রসবার আঁকে। Z-fighting বন্ধ করতে ফ্রেমকে ০.০২ মিটার এবং গ্লাসকে ০.০০৬ মিটার অফসেট দেয়।", table_cell),
            Paragraph("<font name='Courier'>glPushMatrix &rarr; glRotatef(rotY) &rarr; drawBox &rarr; glPopMatrix</font>", table_cell)
        ],
        [
            Paragraph("<b>isHouseLocationFree()</b><br/><i>Line: 650</i>", table_cell),
            Paragraph("প্লেয়ারের ক্যামেরা বাড়ির দেয়ালে ধাক্কা খাচ্ছে কিনা তা যাচাই করে। দরজা ছাড়া অন্য কোনো দেয়াল অতিক্রম করতে দেয় না।", table_cell),
            Paragraph("<font name='Courier'>Inverse 2D Rotation &rarr; AABB Boundary Box Checking</font>", table_cell)
        ],
        [
            Paragraph("<b>triggerGhostSummon()</b><br/><i>Line: 545</i>", table_cell),
            Paragraph("কীবোর্ডের [E] বা [J] চাপলে ভূতকে সক্রিয় করে, পজিশন ইনিশিয়ালাইজ করে এবং হরর সাউন্ড সিন্থেসিস অডিও চালায়।", table_cell),
            Paragraph("<font name='Courier'>g_ghost.active = true; playGhostAudio();</font>", table_cell)
        ],
        [
            Paragraph("<b>updateFallingLeaves()</b><br/><i>Line: 2100</i>", table_cell),
            Paragraph("প্রতি ফ্রেমে ৪৫টি পাতার পজিশন ও ৩ডি রোটেশন আপডেট করে। মাটিতে পড়লে রিস্পন করে।", table_cell),
            Paragraph("<font name='Courier'>Euler Integration: p = p + v * dt; rot = rot + omega * dt</font>", table_cell)
        ],
        [
            Paragraph("<b>drawDilapidatedInterior()</b><br/><i>Line: 3200</i>", table_cell),
            Paragraph("ভাঙা টেবিল, হেলে পড়া চেয়ার, ভাঙা পায়া, মেঝের ফাটল ও মাকড়সার জাল দিয়ে পুরো ঘরের ধ্বংসপ্রাপ্ত পরিবেশ তৈরি করে।", table_cell),
            Paragraph("<font name='Courier'>Compound Matrix Tilts & Alpha Blended Cobwebs</font>", table_cell)
        ],
        [
            Paragraph("<b>displayCallback()</b><br/><i>Line: 7700</i>", table_cell),
            Paragraph("প্রতি ফ্রেমের মূল রেন্ডারার। স্ক্রিন বাফার ক্লিয়ার করে, ক্যামেরা ভিউ সেট করে, লাইট পজিশন বসায় এবং সব অবজেক্ট ড্র করে বাফার সোয়াপ করে।", table_cell),
            Paragraph("<font name='Courier'>gluLookAt &rarr; setupLights &rarr; drawSceneObjects &rarr; glutSwapBuffers</font>", table_cell)
        ]
    ]
    t_funcs = Table(func_data, colWidths=[120, 245, 155])
    t_funcs.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#0f172a")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#cbd5e1")),
        ('ROWBACKGROUNDS', (0,1), (-1,-1), [colors.HexColor("#ffffff"), colors.HexColor("#f8fafc")]),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_funcs)
    story.append(Spacer(1, 8))

    # SECTION 6: কীবোর্ড কন্ট্রোলস ও ভাইভা ডেমো টেবিল
    story.append(Paragraph("৬. কীবোর্ড কন্ট্রোলস ও ভাইভা ডেমো তালিকা (Controls Cheat-Sheet)", h1_style))
    ctrl_data = [
        [Paragraph("<b>বাটন (Key)</b>", table_cell_bold), Paragraph("<b>অ্যাকশন / ডেমোনস্ট্রেশন</b>", table_cell_bold), Paragraph("<b>কোড স্টেট ভ্যারিয়েবল</b>", table_cell_bold)],
        [Paragraph("<b>[E]</b> বা <b>[J]</b>", table_cell), Paragraph("<b>ভূত ডাকা (Summon Ghost Apparition)</b> ও হরর সাউন্ড বাজানো", table_cell), Paragraph("<font name='Courier'>triggerGhostSummon()</font>", table_cell)],
        [Paragraph("<b>[1] [2] [3/F] [4] [5/K] [0]</b>", table_cell), Paragraph("পয়েন্ট, মুনলাইট, ফ্ল্যাশলাইট, উইন্ডো গ্লো, কুমড়া ও মাস্টার লাইট টগল", table_cell), Paragraph("<font name='Courier'>g_light0PointOn ... g_light4CandleOn</font>", table_cell)],
        [Paragraph("<b>[W] [A] [S] [D] + Mouse</b>", table_cell), Paragraph("প্রথম ব্যক্তি দৃষ্টিকোণ থেকে হাঁটা (দরজা দিয়ে ঘরে ঢোকা)", table_cell), Paragraph("<font name='Courier'>processKeyboardInput()</font>", table_cell)],
        [Paragraph("<b>[B]</b>", table_cell), Paragraph("বারান্দার ঝুলন্ত বাল্বের হারমোনিক দুলন ও ফ্লিকার অন/অফ", table_cell), Paragraph("<font name='Courier'>g_bulbSwayEnabled = !g_bulbSwayEnabled</font>", table_cell)],
        [Paragraph("<b>[C]</b>", table_cell), Paragraph("সিনেমেটিক অটো-ট্যুর ক্যামেরা অন/অফ (Auto Tour Presentation)", table_cell), Paragraph("<font name='Courier'>g_cinematicMode = !g_cinematicMode</font>", table_cell)],
        [Paragraph("<b>[L]</b>", table_cell), Paragraph("ম্যানুয়াল বজ্রপাত (Lightning Strike) ও আকাশের উজ্জ্বল আলো", table_cell), Paragraph("<font name='Courier'>triggerLightning()</font>", table_cell)],
        [Paragraph("<b>[T] / [G] / [M]</b>", table_cell), Paragraph("টেক্সচার অন/অফ / কুয়াশা (Fog) অন/অফ / অ্যাম্বিয়েন্ট সাউন্ড অন/অফ", table_cell), Paragraph("<font name='Courier'>g_texEnabled / g_fogEnabled / playAmbientAudio</font>", table_cell)],
        [Paragraph("<b>[P] / [R]</b>", table_cell), Paragraph("স্ক্রিনশট সংরক্ষণ (.bmp) / ক্যামেরা ভিউ রিসেট", table_cell), Paragraph("<font name='Courier'>saveScreenshot() / g_cam reset</font>", table_cell)]
    ]
    t_ctrl = Table(ctrl_data, colWidths=[120, 245, 155])
    t_ctrl.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#1e3a8a")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#cbd5e1")),
        ('ROWBACKGROUNDS', (0,1), (-1,-1), [colors.HexColor("#ffffff"), colors.HexColor("#f8fafc")]),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
    ]))
    story.append(t_ctrl)
    story.append(Spacer(1, 10))

    # SECTION 7: স্যারদের ইনস্ট্যান্ট লাইভ এডিটিং নির্দেশিকা
    story.append(Paragraph("৭. স্যারদের লাইভ এডিটিং নির্দেশিকা (Instant Live-Code Modification Guide)", h1_style))
    p_live = (
        "ভাইভাতে স্যাররা যদি কোনো অবজেক্টের অবস্থান, সাইজ বা রঙ পরিবর্তন করতে বলেন, তবে নিচের জায়গায় সাথে সাথে এডিট করবেন:<br/>"
        "&bull; <b>গাড়ির অবস্থান বদলাতে:</b> <font name='Courier'>src/main.cpp</font> এর লাইন ~7610 তে যান &rarr; <font name='Courier'>drawHauntedCar(7.4f, -5.2f, -32.0f)</font> এর 7.4f (X) বা -5.2f (Z) চেঞ্জ করুন।<br/>"
        "&bull; <b>বাড়ির দেয়াল/রুমের সাইজ বাড়াতে-কমাতে:</b> <font name='Courier'>drawHouse()</font> (লাইন ~2840) এ <font name='Courier'>drawHouseWing(-6.0f, 0.0f, 0.0f, 6.2f, 4.8f, 8.2f)</font> এর W/H/D মান পরিবর্তন করুন।<br/>"
        "&bull; <b>জানালার আলোর রঙ পরিবর্তন করতে:</b> <font name='Courier'>drawHouseWindow()</font> (লাইন ~2780) এ <font name='Courier'>glColor4f(1.0f, 0.72f, 0.25f, 1.0f)</font> পরিবর্তন করে পছন্দমতো রঙ দিন (যেমন গ্রিন ভূতুড়ে আলোর জন্য 0.2f, 1.0f, 0.3f)।<br/>"
        "&bull; <b>টেলিফোন খুঁটির হেলানো কোণ বদলাতে:</b> <font name='Courier'>drawTelephonePole(9.2f, -4.5f, 6.5f)</font> এর 6.5f মানটি বাড়িয়ে 15.0f বা কমিয়ে 0.0f করুন।"
    )
    story.append(Paragraph(p_live, body_style))

    # Build Document
    doc.build(story, canvasmaker=BanglaNumberedCanvas)
    print(f"Bangla PDF Report successfully created: {filename}")

if __name__ == '__main__':
    build_bangla_pdf()
