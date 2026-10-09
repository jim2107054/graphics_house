# 🏰 OpenGL 3D Horror Project — Complete Codebase Architecture & Customization Master Guide
> **প্রজেক্টের সম্পূর্ণ কোডবেস গাইড: প্রতিটি অবজেক্ট, ফাংশন, প্যারামিটার ও ভ্যালু পরিবর্তনের নির্দেশিকা**  
> শিক্ষক/পরীক্ষক (Sir/Examiner) ভাইভায় বা ডেমো দেখার সময় যেকোনো অবজেক্ট, আলো, অ্যানিমেশন বা প্যারামিটার পরিবর্তন করতে বললে যাতে কোডের সঠিক ফাইলে গিয়ে কয়েক সেকেন্ডের মধ্যে তা পরিবর্তন করা যায়, তার জন্য এই মাস্টার রেফারেন্স তৈরি করা হয়েছে।

---

## 📑 সূচিপত্র (Table of Contents)
1. [কোডবেসের আর্কিটেকচার ও ডিরেক্টরি কাঠামো](#-১-কোডবেসের-আর্কিটেকচার-ও-ডিরেক্টরি-কাঠামো)
2. [কুইক ভাইভা চিট-শিট: স্যার যে ভ্যালুগুলো পরিবর্তন করতে বলতে পারেন](#-২-কুইক-ভাইভা-চিট-শিট-স্যার-যে-ভ্যালুগুলো-পরিবর্তন-করতে-বলতে-পারেন)
3. [প্রতিটি অবজেক্টের বিস্তারিত ফাংশন ও প্যারামিটার গাইড](#-৩-প্রতিটি-অবজেক্টের-বিস্তারিত-ফাংশন-ও-প্যারামিটার-গাইড)
   - [৩.১ গাছপালা ও বনায়ন (Vegetation)](#৩১-গাছপালা-ও-বনায়ন-srcentitiesvegetationcpp)
   - [৩.২ ঝরে পড়া পাতা ও বাদুড়ের ঝাঁক (Particles: Leaves & Bats)](#৩২-ঝরে-পড়া-পাতা-ও-বাদুড়ের-ঝাঁক-srcentitiesparticlescpp)
   - [৩.৩ চলমান ৩ডি মেঘমালা ও ছায়া (Clouds)](#৩৩-চলমান-৩ডি-মেঘমালা-ও-ছায়া-srcentitiescloudscpp)
   - [৩.৪ ভিক্টোরিয়ান ভুতুড়ে বাড়ি ও কাঠের সিঁড়ি (House & Staircase)](#৩৪-ভিক্টোরিয়ান-ভুতুড়ে-বাড়ি-ও-কাঠের-সিঁড়ি-srcentitieshousecpp)
   - [৩.৫ প্রাণীসমূহ: পেঁচা ও কালো বিড়াল (Creatures: Owl & Cat)](#৩৫-প্রাণীসমূহ-পেঁচা-ও-কালো-বিড়াল-srcentitiescreaturescpp)
   - [৩.৬ ভিন্টেজ গাড়ি ও কুমড়ো (Props: Car & Pumpkins)](#৩৬-ভিন্টেজ-গাড়ি-ও-কুমড়ো-srcentitiespropscpp)
   - [৩.৭ গোরস্থান ও সমাধি (Graveyard)](#৩৭-গোরস্থান-ও-সমাধি-srcentitiesgraveyardcpp)
   - [৩.৮ ভূপ্রকৃতি, রাস্তা ও ক্লাটার (Terrain & Roads)](#৩৮-ভূপ্রকৃতি-রাস্তা-ও-পরিবেশগত-ক্লাটার-srcentitiesterraincpp)
   - [৩.৯ চাঁদ ও তারামণ্ডল (Sky & Stars)](#৩৯-চাঁদ-ও-তারামণ্ডল-srcentitiesskyandstarscpp)
   - [৩.১০ আলো ও বজ্রপাত (Lighting & Lightning)](#৩১০-আলো-ও-বজ্রপাত-srcgraphicslightingcpp--srcmaincpp)
   - [৩.১১ প্রজেকশন ছায়া (Shadows)](#৩১১-প্ল্যানার-প্রজেকশন-ছায়া-srcgraphicsshadowcpp)
   - [৩.১২ বেসিক জিওমেট্রি শেপস (Primitives)](#৩১২-বেসিক-৩ডি-শেপস-srcgraphicsprimitivescpp)
   - [৩.১৩ ফং ম্যাটেরিয়ালস (Materials)](#৩১৩-ফং-ম্যাটেরিয়াল-সিস্টেম-srcgraphicsmaterialh)
   - [৩.১৪ টেক্সচার ম্যানেজার (Texture Manager)](#৩১৪-টেক্সচার-ম্যানেজার-srcgraphicstexturemanagercpp)
   - [৩.১৫ শব্দ ও অডিও সিস্টেম (Audio System)](#৩১৫-শব্দ-ও-অডিও-সিস্টেম-srcaudioaudiosystemcpp)
   - [৩.১৬ ক্যামেরা ও সিনেমাটিক ট্যুর (Camera & Tour)](#৩১৬-ক্যামেরা-ও-১০-ধাপের-সিনেমাটিক-ট্যুর-srccorecameracpp)
   - [৩.১৭ ইনপুট ও কিবোর্ড হ্যান্ডলার (Input Handling)](#৩১৭-ইনপুট-ও-কিবোর্ড-হ্যান্ডলার-srccoreinputcpp)
   - [৩.১৮ অন-স্ক্রিন ইন্টারফেস (HUD & Screen Grading)](#৩১৮-অন-স্ক্রিন-ইন্টারফেস-srcuihudcpp)
   - [৩.১৯ স্ক্রিনশট সিস্টেম (Screenshot)](#৩১৯-স্ক্রিনশট-সিস্টেম-srcuiscreenshotcpp)
   - [৩.২০ গ্লোবাল কনফিগারেশন ও মেইন লুপ (Config & Main Pipeline)](#৩২০-গ্লোবাল-কনফিগারেশন-ও-মেইন-লুপ-srccoreconfigcpp--srcmaincpp)
4. [কিবোর্ড কন্ট্রোলস তালিকা](#-৪-কিবোর্ড-কন্ট্রোলস-তালিকা-keyboard-shortcuts)
5. [কম্পাইল ও রান করার কমান্ড](#-৫-কম্পাইল-ও-রান-করার-কমান্ড)

---

## 📁 ১. কোডবেসের আর্কিটেকচার ও ডিরেক্টরি কাঠামো

```
opengl-project/
├── include/                 # OpenGL, FreeGLUT ও লাইব্রেরি হেডার ফাইল (.h)
├── lib/                     # লিংকার লাইব্রেরি (.a / .lib)
├── textures/                # টেক্সচার ফাইলসমূহ (.png, .jpg)
└── src/
    ├── main.cpp             # মেইন এন্ট্রি পয়েন্ট, গেম লুপ, ডিসপ্লে ও রেন্ডারিং পাইপলাইন
    ├── core/                # ইঞ্জিন কোর
    │   ├── Camera.h/.cpp    # FPS ক্যামেরা, ফ্রি মুভমেন্ট ও ১০-ধাপের সিনেমাটিক ট্যুর
    │   ├── Config.h/.cpp    # গ্লোবাল ভেরিয়েবল, লাইট ও অ্যানিমেশন ফ্ল্যাগ
    │   └── Input.h/.cpp     # কি-বোর্ড ও মাউস হ্যান্ডলার (WASD, 1-5, F, C, T, ইত্যাদি)
    ├── entities/            # সিনের সমস্ত ৩ডি অবজেক্ট ও প্রাণী
    │   ├── House.h/.cpp     # ভিক্টোরিয়ান বাড়ি, নিচতলা, দোতলা অ্যাটিক ও ১৫-ধাপের সিঁড়ি
    │   ├── Vegetation.h/.cpp# উঁচু ওক গাছ ও পাইন গাছের অ্যালগরিদম
    │   ├── Particles.h/.cpp # গাছের ৩ডি পাতা ঝরে পড়া ও বাদুড়ের ঝাঁক
    │   ├── Creatures.h/.cpp # কালো বিড়াল (Patrol) ও পেঁচা (Proximity Escape Flight)
    │   ├── Clouds.h/.cpp    # বাস্তবসম্মত চলমান ৩ডি মেঘমালা ও ছায়া
    │   ├── Props.h/.cpp     # ১৯৫০-এর ভিন্টেজ গাড়ি, জ্যাক-ও-ল্যান্টার্ন কুমড়ো
    │   ├── Graveyard.h/.cpp # সেল্টিক ক্রস, কবর ও পাথরের সমাধি
    │   ├── Terrain.h/.cpp   # পাহাড়ি আঁকাবাঁকা মাটি ও ভেজা পাথুরে রাস্তা
    │   └── SkyAndStars.h/.cpp # আলোকিত চাঁদ ও তারামণ্ডল
    ├── graphics/            # গ্রাফিক্স পাইপলাইন, ম্যাটেরিয়াল ও লাইটিং
    │   ├── Lighting.h/.cpp  # বারান্দার ঝুলন্ত বাল্ব, চাঁদ ও বজ্রপাত
    │   ├── Material.h       # ফং (Phong) অ্যাম্বিয়েন্ট, ডিফ্যুজ ও স্পেকুলার ম্যাটেরিয়ালস
    │   ├── Primitives.h/.cpp# বেসিক ৩ডি শেপস (Box, Cylinder, Sphere, Beam)
    │   ├── Shadow.h/.cpp    # চাঁদের আলোর প্ল্যানার প্রজেকশন শ্যাডো
    │   └── TextureManager.h/.cpp # টেক্সচার লোডার ও ম্যাপিং
    ├── audio/               # সাউন্ড ইঞ্জিন
    │   └── AudioSystem.h/.cpp # অ্যাম্বিয়েন্ট উইন্ড ও থান্ডার সাউন্ড ইফেক্টস
    └── ui/                  # অন-স্ক্রিন ইন্টারফেস
        ├── HUD.h/.cpp       # ২ডি অন-স্ক্রিন স্ট্যাটাস বার, ক্রসহেয়ার ও কন্ট্রোলস
        └── Screenshot.h/.cpp# হাই-রেজোলিউশন স্ক্রিনশট ক্যাপচার
```

---

## ⚡ ২. কুইক ভাইভা চিট-শিট: স্যার যে ভ্যালুগুলো পরিবর্তন করতে বলতে পারেন

| কোন জিনিস পরিবর্তন করবেন? | ফাইলের নাম | ফাংশন / লাইন | কোন ভেরিয়েবল বদলাবেন? | নতুন মান কী দেবেন? |
| :--- | :--- | :--- | :--- | :--- |
| **১. মেঘের গতি (Cloud Speed)** | `src/entities/Clouds.cpp` | Line 27: `G_CLOUDS` | `speed` | দ্রুত করতে `1.2f`, ধীর করতে `0.20f` |
| **২. মেঘের আকার (Cloud Scale)** | `src/entities/Clouds.cpp` | Line 27: `G_CLOUDS` | `scale` | বড় করতে `1.8f`, ছোট করতে `0.8f` |
| **৩. বাদুড়ের আকার (Bat Size)** | `src/entities/Particles.cpp`| Line 530: `G_BAT_SWARM` | `scale` | বড় করতে `1.8f - 2.2f`, ছোট করতে `0.8f` |
| **৪. বাদুড়ের ওড়ার সময় (Bat Duration)**| `src/entities/Particles.cpp`| Line 530: `G_BAT_SWARM` | `cycleDuration` | ধীর করতে `35.0f`, দ্রুত করতে `15.0f` |
| **৫. বাদুড়ের ডানার ঝাপটানি** | `src/entities/Particles.cpp`| Line 530: `G_BAT_SWARM` | `flapFreq` | দ্রুত করতে `14.0f`, স্বাভাবিক `7.5f` |
| **৬. গাছের পাতার সংখ্যা (Leaf Count)** | `src/entities/Particles.cpp`| Line 11 | `MAX_FALLING_LEAVES` | বেশি করতে `40`, কম করতে `10` |
| **৭. পাতা পড়ার গতি (Leaf Fall Speed)** | `src/entities/Particles.cpp`| Line 71: `spawnLeafFromTree`| `l.vy` | দ্রুত নামাতে `0.5f`, আস্তে নামাতে `0.15f` |
| **৮. পাতার আকার (Leaf Size)** | `src/entities/Particles.cpp`| Line 81: `spawnLeafFromTree`| `l.size` | বড় করতে `0.35f`, ছোট করতে `0.15f` |
| **৯. বাতাসের ধাক্কা (Wind Drift)** | `src/entities/Particles.cpp`| Line 106: `updateFallingLeaves`| `windSpeedX` | বাতাস বাড়াতে `-0.7f`, কমাতে `-0.1f` |
| **১০. গাছের উচ্চতা (Tree Height)** | `src/entities/Vegetation.cpp`| Line 284: `drawAllTrees()` | `height` | আরও লম্বা করতে `9.0f`, ছোট করতে `4.5f` |
| **১১. গাছের গুঁড়ির ঘনত্ব (Trunk)** | `src/entities/Vegetation.cpp`| Line 284: `drawAllTrees()` | `trunkRadius` | মোটা করতে `0.35f`, চিকন করতে `0.18f` |
| **১২. ফ্ল্যাশলাইট আলো (Flashlight Intensity)** | `src/main.cpp` | Line 47 & 170 | `flashDiff[4]` | আরও উজ্জ্বল করতে `{ 3.5f, 3.5f, 3.5f, 1.0f }` |
| **১৩. ফ্ল্যাশলাইটের কোণ (Spot Cutoff)** | `src/main.cpp` | Line 44: `initOpenGL()` | `GL_SPOT_CUTOFF` | ছড়াতে `35.0f`, সরু ফোকাস `15.0f` |
| **১৪. চাঁদের আলোর উজ্জ্বলতা (Moon Intensity)**| `src/main.cpp` | Line 145: `render3DScene()` | `mDiff[4]` | উজ্জ্বল নীল `{0.4f, 0.5f, 0.7f, 1.0f}` |
| **১৫. বারান্দার বাল্বের দোলা (Bulb Swing)** | `src/graphics/Lighting.cpp` | Line 10: `updateBulbMotion()`| `swingAmp` | বেশি দোলাতে `0.35f`, স্থির রাখতে `0.02f` |
| **১৬. পেঁচার উড়ে যাওয়ার দূরত্ব (Owl Trigger)**| `src/entities/Creatures.cpp` | Line 761: `drawPerchedOwl()` | `camDist < 3.2f` | দূরে থাকতেই উড়াতে `6.0f`, কাছে নিতে `1.8f` |
| **১৭. পেঁচার ওড়ার গতি (Owl Flight Speed)** | `src/entities/Creatures.cpp` | Line 927: `drawPerchedOwl()` | `escX, escY, escZ` | `2.4f * t` বাড়ালে গতি বাড়বে |
| **১৮. বিড়ালের হাঁটার গতি (Cat Stride)** | `src/entities/Creatures.cpp` | Line 329: `drawProwlingBlackCat()`| `walkPhase` | দ্রুত পা ফেলতে `12.0f`, ধীর করতে `5.0f` |
| **১৯. প্লেয়ারের হাঁটার গতি (Walk Speed)** | `src/core/Camera.cpp` | Line 10: `Camera g_cam` | `moveSpeed` | দ্রুত হাঁটতে `20.0f`, আস্তে হাঁটতে `8.0f` |
| **২০. ক্যামেরার ফিল্ড অব ভিউ (FOV)** | `src/core/Camera.cpp` | Line 12: `Camera g_cam` | `fov` | ওয়াইড ভিউ `65.0f`, সিনেমাটিক `45.0f` |

---

## 🛠️ ৩. প্রতিটি অবজেক্টের বিস্তারিত ফাংশন ও প্যারামিটার গাইড

---

### ৩.১ গাছপালা ও বনায়ন (`src/entities/Vegetation.cpp`)
* **মূল দায়িত্ব:** সিনের চারপাশের ওক গাছ, সূঁচালো পাইন গাছ, ভুতুড়ে আঁকাবাঁকা গাছ ও গাছের শিকড় তৈরি করা।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `drawNaturalDeciduousTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, int foliageType, float colorTint)`
     - `x, z`: মাটিতে গাছের অবস্থান।
     - `trunkRadius`: গুঁড়ির ব্যাসার্ধ (ডিফল্ট `0.21f - 0.26f`)।
     - `height`: গাছের মোট উচ্চতা (ডিফল্ট `6.5m - 8.2m`)।
     - `seed`: র্যান্ডম ডালপালার আকৃতি নির্ধারণ করে।
     - `foliageType`: `0` = গাঢ় সবুজ, `1` = স্বর্ণালী শরৎকাল, `2` = গাঢ় ওক।
  2. `drawNaturalPineTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed)`
     - শঙ্কু আকৃতির ত্রিভুজাকার সূঁচালো পাইন গাছ আঁকে।
  3. `drawOrganicCreepyTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, float colorTint)`
     - পাতা ছাড়া ভুতুড়ে কঙ্কালসার ডালপালা আঁকে।
  4. `drawRootFlare(float trunkR, TreeRNG& rng)`
     - গাছের গোড়ায় মাটির ওপর ছড়িয়ে থাকা শিকড় আঁকে।
  5. `drawNaturalFoliageCluster(float radius, TreeRNG& rng, int foliageType)`
     - পাতার থোকা (Lobe) তৈরি করে।
  6. `drawAllTrees()`
     - সিনের মোট ২৬টি গাছ ৪টি জোনে প্লেস করে।

---

### ৩.২ ঝরে পড়া পাতা ও বাদুড়ের ঝাঁক (`src/entities/Particles.cpp`)
* **মূল দায়িত্ব:** গাছের ক্যানোপি থেকে বাস্তবসম্মত ৩ডি পাতা খসে পড়া এবং আকাশে ২২টি বাদুড়ের দল বেঁধে ওড়া।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `spawnLeafFromTree(FallingLeafParticle& l, int treeIdx, bool initialStagger)`
     - `G_LEAF_TREES[6]`: যে ৬টি আসল পাতাযুক্ত গাছ থেকে পাতা পড়বে তাদের পজিশন ও উচ্চতা।
     - `l.vy`: নিচে পড়ার গতি (ডিফল্ট `0.16f - 0.30f m/s`)।
     - `l.size`: পাতার আকার (ডিফল্ট `0.22f - 0.32f`)।
     - `l.r, l.g, l.b`: ৪টি খাঁটি শরতের রঙ (লাল, গোল্ডেন, কমলা, বাদামী)।
  2. `drawRealistic3DLeaf(float length, float width, float r, float g, float b, int variant)`
     - পাতার ৩ডি বোঁটা (Petiole), খাঁজকাটা ওক ও ম্যাপল ব্লেড এবং প্রধান ও শাখা শিরা রেন্ডার করে।
  3. `updateFallingLeaves(float dt)`
     - বাতাসে পাতার ডানে-বামে দোলা (`swayPhase`), টার্বুলেন্স ও মাটিতে পড়া নিয়ন্ত্রণ করে।
  4. `drawRealisticBat(float x, float y, float z, float yaw, float pitch, float roll, float flapAngle, float scale)`
     - লোমশ শরীর, কান, চোখ, দাঁত এবং জয়েন্টসহ ৩ডি ডানা আঁকে।
  5. `evalSwarmBatAtProgress(const BatSwarmDef& b, float s, float tParam, float& outX, float& outY, float& outZ)`
     - আকাশ পারাপার (`startX = -52.0m` থেকে `endX = +52.0m`) হিসাব করে।
  6. `drawAllBats()`
     - ২২টি বাদুড়ের ঝাঁক আকাশে ড্র করে।

---

### ৩.৩ চলমান ৩ডি মেঘমালা ও ছায়া (`src/entities/Clouds.cpp`)
* **মূল দায়িত্ব:** আকাশে ধীরগতির রূপালী আভা যুক্ত মেঘ এবং মাটিতে মেঘের চলমান ছায়া তৈরি করা।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `drawDynamicClouds()`
     - `G_CLOUDS[6]`: ৬টি মেঘের অ্যারে।
     - `speed`: অনুভূমিক গতি (ডিফল্ট `0.22f - 0.46f`)।
     - `scale`: মেঘের আয়তন (ডিফল্ট `0.90f - 1.40f`)।
     - `y`: আকাশে উচ্চতা (ডিফল্ট `26.5m - 36.5m`)।
  2. `drawAtmosphericCloudLobe(float lx, float ly, float lz, float lr, float baseScale, float alphaMul, float rimIntensity)`
     - চাঁদের আলোর কোণ অনুযায়ী মেঘের কানায় রূপালী আভা (Silver lining) তৈরি করে।
  3. `drawCloudShadowCasters()`
     - মাটিতে নরম ডিম্বাকৃতির মেঘের ছায়া ফেলে।

---

### ৩.৪ ভিক্টোরিয়ান ভুতুড়ে বাড়ি ও কাঠের সিঁড়ি (`src/entities/House.cpp`)
* **মূল দায়িত্ব:** ৩ তলা ভিক্টোরিয়ান গথিক ম্যানশন, কাঠের ফ্লোরিং, নিচতলা, দোতলা ও ওঠার সিঁড়ি।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `drawHouse()`
     - দেয়াল, ঢালু ছাদ, চিমনি, ছাদের চূড়ার স্পায়ার, জানালা ও ঝুলন্ত বারান্দা আঁকে।
  2. `drawStaircase()`
     - নিচতলা থেকে দোতলায় ওঠার বাস্তবসম্মত ১৫-ধাপের কাঠের সিঁড়ি ও রেলিং আঁকে।
  3. `drawHouseInterior()`
     - নিচতলার পার্লার, ফায়ারপ্লেস, দেয়ালঘড়ি, সোফা, বুকশেলফ ও ঝুলন্ত মাকড়সার জাল।
  4. `drawSecondFloorInterior()`
     - দোতলার অ্যাটিক স্টাডি রুম, কাঠের বেড, আলকেমিস্ট ডেস্ক, মোমবাতি ও সিলিং লণ্ঠন।
  5. `drawHouseWindow(x, y, z, width, height, rotY, hasArch, hasCrossMuntin)`
     - গথিক খিলানযুক্ত কাঠের ফ্রেমের জানালা।
  6. `drawCobweb(x, y, z, size, rotY)` ও `drawOutdoorCobweb(...)`
     - কোণায় ঝুলন্ত জ্যামিতিক মাকড়সার জাল।

---

### ৩.৫ প্রাণীসমূহ: পেঁচা ও কালো বিড়াল (`src/entities/Creatures.cpp`)
* **মূল দায়িত্ব:** জীবন্ত প্রাণী যারা প্লেয়ার ও পরিবেশের সাথে রিয়্যাক্ট করে।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `drawProwlingBlackCat()`
     - ৪ পায়ের কোয়াড্রুপেড ওয়াক সাইকেল (`walkPhase`), দোদুল্যমান লেজ ও জ্বলজ্বলে সবুজ চোখ (`GL_EMISSION`)।
     - গোরস্থানের রেলিংয়ে টহল দেয়, বসে এবং ১৮০° ঘুরে ফিরে আসে।
  2. `drawPerchedOwl()`
     - কাঠের খুঁটিতে বসে মাথা ঘোরানো (`headYaw`) এবং চোখের পলক ফেলা।
     - `camDist < 3.2f`: প্লেয়ার ৩.২ মিটারের কাছে গেলে ডানা মেলে আকাশে উড়ে পালিয়ে যায়।
  3. `resetOwl()`
     - পেঁচার ফ্লাইট স্টেট রিসেট করে আবার খুঁটিতে ফিরিয়ে আনে।

---

### ৩.৬ ভিন্টেজ গাড়ি ও কুমড়ো (`src/entities/Props.cpp`)
* **মূল দায়িত্ব:** ধ্বংসপ্রাপ্ত ভিন্টেজ গাড়ি ও জ্বলন্ত হ্যালোউইন কুমড়ো।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `drawRustedCar(float x, float z, float rotY)`
     - ১৯৫০-এর আমেরিকান সেডান, খোলা হুড, ভি৮ ইঞ্জিন ব্লক, ব্যাটারি, স্টিয়ারিং হুইল ও ক্রোম ট্রিম।
  2. `drawPumpkin(float x, float y, float z, float scale, float rotY, int faceStyle)`
     - খাঁজকাটা কুমড়োর খোলস, খোদাই করা চোখ-মুখ এবং ভেতরের মোমবাতির কাঁপুনিযুক্ত আলো।
  3. `drawPumpkinArray()`
     - বাড়ির বারান্দায় ও সিঁড়ির পাশে সাজানো কুমড়োর সারি।
  4. `drawTelephonePole(float x, float z, float rotY)`
     - কাঠের টেলিফোন খুঁটি, ক্রসার্ম, ইনসুলেটর ও ঝুলন্ত তার।

---

### ৩.৭ গোরস্থান ও সমাধি (`src/entities/Graveyard.cpp`)
* **মূল দায়িত্ব:** ঐতিহাসিক গোরস্থান ও প্রাচীন সমাধিফলক তৈরি।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `drawArchedHeadstone(x, z, scale, rotY, lean, leanDir, stoneType)`
     - খোদাই করা পাথর ও ক্রসযুক্ত হেভিস্টোন।
  2. `drawCelticCrossGrave(x, z, scale, rotY, lean, leanDir)`
     - খোদাই করা বৃত্তযুক্ত সেল্টিক ক্রস সমাধি।
  3. `drawStoneSarcophagusGrave(x, z, scale, rotY, tilt, lidAjar)`
     - পাথরের শবাধার, যার ঢাকনা একটু ফাঁক হয়ে ভেতরের অন্ধকার দেখা যায়।
  4. `drawEarthBurialMound(x, z, scale, rotY)`
     - নতুন খোঁড়া মাটির ঢিবি।
  5. `drawDetailedBrokenFenceSection(x, z, rotY)` ও `drawBrokenFence()`
     - ভাঙা ও বাঁকা লোহার স্পাইকযুক্ত গোরস্থানের সীমানা রেলিং।

---

### ৩.৮ ভূপ্রকৃতি, রাস্তা ও পরিবেশগত ক্লাটার (`src/entities/Terrain.cpp`)
* **মূল দায়িত্ব:** আঁকাবাঁকা অসমতল মাটি, বৃষ্টির পানি জমা গর্ত, নুড়িপাথর ও ছড়িয়ে থাকা ধ্বংসাবশেষ।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `getTerrainHeight(float x, float z)`
     - ম্যাথমেটিক্যাল সাইন-কোসাইন ঢেউ দিয়ে যেকোনো বিন্দুর মাটির উচ্চতা নির্ধারণ করে।
  2. `getRoadCenterX(float pz)`
     - মাটির ওপর দিয়ে যাওয়া এস-কার্ভ (S-curve) রাস্তার পথ নির্ধারণ করে।
  3. `drawPuddle(float cx, float cz, float radiusX, float radiusZ, float rotAngle)`
     - বৃষ্টির ভেজা চকচকে পানির গর্ত যা চাঁদের আলো রিফ্লেক্ট করে।
  4. `drawDenseGrassField()`
     - মাটিতে ছড়িয়ে থাকা বুনো ঘাসের গোছা।
  5. `drawEnvironmentalClutter()`
     - ছড়িয়ে থাকা ভাঙা কাঠের বাক্স (`drawBrokenCrate`), পুরনো ব্যারেল (`drawOldBarrel`), মরচে ধরা বালতি (`drawRustyBucket`), ইটের টুকরো (`drawScatteredBricks`), পুরনো হেলানো টেবিল ও চেয়ার।

---

### ৩.৯ চাঁদ ও তারামণ্ডল (`src/entities/SkyAndStars.cpp`)
* **মূল দায়িত্ব:** বাস্তবসম্মত চাঁদের গোলক, ক্রেটার, আলোর আভা এবং জ্বলজ্বলে তারামণ্ডল।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `initStars()`
     - আকাশে ৫০০টি র্যান্ডম তারার পজিশন ও সাইজ তৈরি করে।
  2. `drawMoonAndStars()`
     - চাঁদের টেক্সচার্ড ৩ডি স্ফিয়ার, তারার মিটিমিটি আলো ও চাঁদের চারপাশের নরম বিলবোর্ড হ্যালো (`drawBillboardHalo`) রেন্ডার করে।

---

### ৩.১০ আলো ও বজ্রপাত (`src/graphics/Lighting.cpp` & `src/main.cpp`)
* **মূল দায়িত্ব:** OpenGL এর ৬টি লাইট সোর্স নিয়ন্ত্রণ ও আকাশে বজ্রপাত।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `GL_LIGHT0` (বারান্দার পয়েন্ট লাইট): ঝুলন্ত বাল্বের আলো, বাতাসে পেন্ডুলামের মতো দোলে (`updateBulbMotion`)।
  2. `GL_LIGHT1` (চাঁদের আলো): ডিরেকশনাল শীতল নীল আলো (`g_moonDir`)।
  3. `GL_LIGHT2` (ফ্ল্যাশলাইট): খেলোয়াড়ের হাতের টর্চ; `GL_SPOT_CUTOFF = 25.0f` এবং `flashDiff[4] = {2.6f, 2.5f, 2.2f, 1.0f}`।
  4. `GL_LIGHT3` (জানালার অ্যাম্বিয়েন্ট): নিচতলার কাঁচ দিয়ে বের হওয়া উষ্ণ আলো।
  5. `GL_LIGHT4` (দোতলার মোমবাতি): অ্যাটিকের ডেস্কের ক্যান্ডেলব্রার আলো।
  6. `GL_LIGHT5` (দোতলার লণ্ঠন): সিলিংয়ের ঝুলন্ত লণ্ঠন।
  7. `triggerLightning()`: আকাশে বিদ্যুতের ঝলকানি ও তীব্র আলো ফেলে।

---

### ৩.১১ প্ল্যানার প্রজেকশন ছায়া (`src/graphics/Shadow.cpp`)
* **মূল দায়িত্ব:** ৩ডি অবজেক্টের চাঁদের আলোয় মাটিতে কালো ছায়া প্রজেক্ট করা।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `buildShadowMatrix(float shadowMat[16], const float groundPlane[4], const float lightPos[4])`
     - গ্রাউন্ড প্লেনের ওপর ৪x৪ শ্যাডো প্রজেকশন ম্যাট্রিক্স তৈরি করে।
  2. `renderPlanarShadows()`
     - বাড়ি, গাছ, গাড়ি ও বেড়ার জ্যামিতিকে মাটিতে ছায়া হিসেবে আঁকে।

---

### ৩.১২ বেসিক ৩ডি শেপস (`src/graphics/Primitives.cpp`)
* **মূল দায়িত্ব:** সিনের সমস্ত জটিল অবজেক্ট বানাতে প্রয়োজনীয় কোর প্রিমিটিভস।
* **ফাংশনসমূহ:**
  - `drawBox(width, height, depth, tileU, tileV)`
  - `drawBeveledBox(width, height, depth, bevel, tileU, tileV)`
  - `drawCylinder(baseRadius, topRadius, height, slices, tileU, tileV)`
  - `drawSphere(radius, slices, stacks, tileU, tileV)`
  - `drawPrismRoof(width, height, length, tileU, tileV)`
  - `drawSteepleSpire(baseRadius, height, facets, tileU, tileV)`
  - `drawBillboardHalo(x, y, z, radius, r, g, b, maxAlpha)`
  - `drawVolumetricFlashlightBeam(posX, posY, posZ, dirX, dirY, dirZ)`

---

### ৩.১৩ ফং ম্যাটেরিয়াল সিস্টেম (`src/graphics/Material.h`)
* **মূল দায়িত্ব:** বাস্তবসম্মত আলোক প্রতিফলন (Ambient, Diffuse, Specular, Shininess)।
* **ম্যাটেরিয়াল প্রিসেটসমূহ:**
  - `MAT_DARK_WOOD`: পুরানো কালো কাঠ।
  - `MAT_WEATHERED_WALL`: রোদে-বৃষ্টিতে ক্ষয়ে যাওয়া দেয়াল।
  - `MAT_ROOF_SHINGLE`: ছাদের পাথুরে টাইলস।
  - `MAT_STONE` ও `MAT_MOSS_STONE`: শ্যাওলা ধরা কবর ও ভিত্তিপ্রস্তর।
  - `MAT_RUSTY_METAL`: গাড়ির মরিচা ধরা লোহা।
  - `MAT_BLACK_IRON`: গোরস্থানের কালো রেলিং।
  - `MAT_CAR_GLASS`: চকচকে কাঁচ (Specular = 128)।
  - `MAT_PUMPKIN_SKIN` ও `MAT_PUMPKIN_GLOW`: কুমড়ো ও ভেতরের উজ্জ্বল আলো।
  - `MAT_BULB_EMISSIVE` ও `MAT_WINDOW_GLOW`: বাল্ব ও জানালার দ্যুতি।
  - `MAT_FOLIAGE_DARK`, `MAT_FOLIAGE_LUSH`, `MAT_FOLIAGE_AUTUMN`, `MAT_PINE_NEEDLES`: বিভিন্ন ঋতুর পাতা।

---

### ৩.১৪ টেক্সচার ম্যানেজার (`src/graphics/TextureManager.cpp`)
* **মূল দায়িত্ব:** টেক্সচার লোড করা অথবা ইমেজ না থাকলে প্রসিডিউরাল টেক্সচার জেনারেট করা।
* **ফাংশনসমূহ:**
  - `bindTexture(TextureID id)`: সক্রিয় টেক্সচার বাইন্ড করে।
  - `generateProceduralTexture(...)`: কোড দিয়ে কাঠ, ইট, ছাদ ও পাথরের টেক্সচার তৈরি করে।
  - `loadSceneTexture(...)`: `.png` বা `.jpg` ফাইল থেকে টেক্সচার লোড করে।

---

### ৩.১৫ শব্দ ও অডিও সিস্টেম (`src/audio/AudioSystem.cpp`)
* **মূল দায়িত্ব:** ভুতুড়ে বাতাসের শব্দ ও মেঘের ডাক (Windows API দিয়ে চালিত)।
* **ফাংশনসমূহ:**
  - `initProceduralAudio()`: সিন্থেসাইজার দিয়ে অডিও বাফার তৈরি করে।
  - `playAmbientAudio()`: ব্যাকগ্রাউন্ডে বাতাসের হাহাকার লুপে বাজায়।
  - `playThunderAudio()`: বজ্রপাতের শব্দ বাজায়।
  - `toggleAudio()`: সাউন্ড মিউট/আনমিউট করে।

---

### ৩.১৬ ক্যামেরা ও ১০-ধাপের সিনেমাটিক ট্যুর (`src/core/Camera.cpp`)
* **মূল দায়িত্ব:** প্রথম ব্যক্তি (FPS) মুভমেন্ট, সিঁড়ি বেয়ে ওপরে ওঠা ও অটোমেটিক সিনেমাটিক ট্যুর।
* **ফাংশনসমূহ ও প্যারামিটার:**
  1. `Camera g_cam`:
     - `speed` (ডিফল্ট `12.0f`): হাঁটার গতি।
     - `sens` (ডিফল্ট `0.12f`): মাউস ঘোরানোর সংবেদনশীলতা।
     - `fov` (ডিফল্ট `55.0f`): ক্যামেরার দৃষ্টিসীমা।
  2. `isHouseLocationFree(worldX, worldZ)`: দেয়ালের সাথে ধাক্কা খাওয়া রোধ করে।
  3. `updateCinematicCamera(float dt)`:
     - 'C' চাপলে ১০টি ধাপে পুরো প্রজেক্ট ঘুরে দেখায়:
       1. গোরস্থান ও চাঁদ দর্শন
       2. আঁকাবাঁকা পথ বেয়ে হাঁটা
       3. ভিন্টেজ গাড়ি পরিদর্শন
       4. বারান্দার ঝুলন্ত বাল্ব ও কুমড়ো
       5. নিচতলার পার্লার ও ফায়ারপ্লেস
       6. ১৫-ধাপের কাঠের সিঁড়ি বেয়ে দোতলায় ওঠা
       7. দোতলার অ্যাটিক, ডেস্ক ও বুকশেলফ
       8. দোতলার বারান্দা থেকে মেঘ ও বাদুড় দর্শন
       9. পেঁচার কাছে যাওয়া ও উড়ে পালানো
       10. রাতের পূর্ণাঙ্গ আকাশ ও বাড়ি দর্শন।

---

### ৩.১৭ ইনপুট ও কিবোর্ড হ্যান্ডলার (`src/core/Input.cpp`)
* **মূল দায়িত্ব:** কিবোর্ডের বোতাম এবং মাউসের ক্লিক ও স্ক্রল হ্যান্ডল করা।
* **ফাংশনসমূহ:**
  - `keyboardDown(unsigned char key, int x, int y)`
  - `keyboardUp(unsigned char key, int x, int y)`
  - `mouseMotion(int x, int y)`
  - `mouseWheel(int button, int dir, int x, int y)`

---

### ৩.১৮ অন-স্ক্রিন ইন্টারফেস (`src/ui/HUD.cpp`)
* **মূল দায়িত্ব:** স্ক্রিনে টেক্সট, ফ্রেমরেট (FPS), ক্রসহেয়ার ও সিনেমাটিক বার আঁকা।
* **ফাংশনসমূহ:**
  - `drawString2D(...)`: স্ক্রিনে টেক্সট প্রিন্ট করে।
  - `drawUIPanel(...)`: ট্রান্সলুসেন্ট ব্যাকগ্রাউন্ড প্যানেল আঁকে।
  - `drawCinematicColorGrade()`: সিনেমাটিক নীল-কালো ভিনিয়েট কালার গ্রেডিং দেয়।
  - `renderSceneHUD()`: বর্তমান লাইটিং স্ট্যাটাস ও বাটন নির্দেশিকা প্রদর্শন করে।

---

### ৩.১৯ স্ক্রিনশট সিস্টেম (`src/ui/Screenshot.cpp`)
* **মূল দায়িত্ব:** 'P' চাপলে প্রজেক্টের বর্তমান ফ্রেম ইমেজ আকারে সেভ করা।
* **ফাংশনসমূহ:**
  - `saveScreenshot(const char* filename)`: ওপেনজিএল ব্যাক-বাফার রিড করে `.bmp` বা `.ppm` ফাইল তৈরি করে।

---

### ৩.২০ গ্লোবাল কনফিগারেশন ও মেইন লুপ (`src/core/Config.cpp` & `src/main.cpp`)
* **মূল দায়িত্ব:** উইন্ডো তৈরি, টাইমার লুপ, ওপেনজিএল স্টেট মেশিন ও ডিসপ্লে পাইপলাইন।
* **ফাংশনসমূহ:**
  - `initOpenGL()`: ডেপথ টেস্ট, ব্লেন্ডিং ও লাইট প্যারামিটার চালু করে।
  - `render3DScene()`: পুরো সিনটি ক্রমানুসারে রেন্ডার করে।
  - `main(int argc, char** argv)`: গ্লুট (GLUT) ইনিশিয়ালাইজ করে গেম লুপ শুরু করে।

---

## 🎮 ৪. কিবোর্ড কন্ট্রোলস তালিকা (Keyboard Shortcuts)

| কি (Key) | কাজ (Function) | ফাইল যেখানে কোড আছে |
| :---: | :--- | :--- |
| **W, A, S, D** | সামনে, বামে, পেছনে, ডানে হাঁটা (সিঁড়ি দিয়ে দোতলায় ওঠা যায়) | `src/core/Camera.cpp` |
| **Mouse Move** | চারদিকে তাকানো (Yaw ও Pitch) | `src/core/Input.cpp` |
| **Mouse Scroll** | ক্যামেরার জুম ইন / জুম আউট (FOV পরিবর্তন) | `src/core/Input.cpp` |
| **Space / Ctrl** | ওপরে ওঠা (Fly Up) / নিচে নামা (Fly Down) | `src/core/Camera.cpp` |
| **`3` বা `F` বা মাউস ক্লিক**| **ফ্ল্যাশলাইট অন/অফ** (যেকোনো অবজেক্ট সরাসরি আলোকিত হয়) | `src/core/Input.cpp` |
| **`1`** | বারান্দার ঝুলন্ত বাল্ব অন/অফ | `src/core/Input.cpp` |
| **`2`** | চাঁদের আলো ও ছায়া অন/অফ | `src/core/Input.cpp` |
| **`4`** | নিচতলার জানালার উষ্ণ আলো অন/অফ | `src/core/Input.cpp` |
| **`5` বা `K`** | জ্যাক-ও-ল্যান্টার্ন কুমড়োর আলো অন/অফ | `src/core/Input.cpp` |
| **`0`** | মাস্টার লাইট টগল (এক ক্লিকে সব লাইট অন/অফ) | `src/core/Input.cpp` |
| **`T`** | টেক্সচার অন/অফ (Texture vs Solid Phong Material) | `src/core/Input.cpp` |
| **`B`** | বারান্দার বাল্বের সুইং ও ফ্লিকার অ্যানিমেশন অন/অফ | `src/core/Input.cpp` |
| **`L`** | আকাশে বিদ্যুৎ চমকানো ও বজ্রপাত ট্রিগার | `src/core/Input.cpp` |
| **`C` বা `U`** | **১০-ধাপের সম্পূর্ণ গাইডেড সিনেমাটিক ট্যুর** (অটো ক্যামেরা ও লাইট ডেমো) | `src/core/Camera.cpp` |
| **`V` বা `I`** | ভিউ সাইকেল (বাহির → নিচতলা পার্লার → দোতলা অ্যাটিক) | `src/core/Camera.cpp` |
| **`H` বা `TAB`** | অন-স্ক্রিন HUD মেনু হাইড / শো | `src/ui/HUD.cpp` |
| **`P`** | হাই-রেজোলিউশন স্ক্রিনশট সেভ করা | `src/ui/Screenshot.cpp` |
| **`ESC`** | প্রজেক্ট বন্ধ করা | `src/core/Input.cpp` |

---

## 🚀 ৫. কম্পাইল ও রান করার কমান্ড

যদি কোনো ফাইলে মান পরিবর্তন করেন, টার্মিনালে নিচের কমান্ড দিয়ে কম্পাইল ও রান করবেন:

```powershell
# ১. যদি main.exe ব্যাকগ্রাউন্ডে চলতে থাকে তবে তা বন্ধ করা:
taskkill /F /IM main.exe

# ২. কম্পাইল কমান্ড:
g++ -std=c++17 src/main.cpp src/stb_image.cpp src/core/*.cpp src/audio/*.cpp src/graphics/*.cpp src/entities/*.cpp src/ui/*.cpp -Iinclude -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -lwinmm -o main.exe

# ৩. রান করা:
.\main.exe
# অথবা সরাসরি:
.\run.bat
```
