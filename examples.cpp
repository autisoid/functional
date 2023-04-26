typedef struct CClass {
	CClass(_In_ int _Age) : m_iAge(_Age) {
		printf("CClass::CClass(int)\n");
	}

	~CClass() {
		printf("CClass::~CClass()\n");
	}

	void Print() {
		printf("Hi my name is Bob and my age is %d\n", this->m_iAge);
	}

	int m_iAge;
} CClass;

void UniquePointerExample() {
	CUniquePointer<CClass> klass = make_unique<CClass>(23);

	klass->Print();

	klass.reset();

	printf("0x%p\n", klass.get());
}

void RandomExample() {
	CUniquePointer<CTrustedRandom> random = make_unique<CTrustedRandom>();

	random->srand(RandomSeed());

	for (int idx = 0; idx < 1024; idx++) {
		printf("%d%s", random->autorand(0, 70000), idx != 1023 ? ", " : "\n");
		random->srand(random->autorand(0, 1023));
	}

	printf("Generating new device...\n");

	CUniquePointer<CTrustedRandom> next_device(random->GenerateNewDevice());

	random.reset();

	for (int idx = 0; idx < 1024; idx++) {
		printf("%d%s", next_device->autorand(0, 100), idx != 1023 ? ", " : "\n");
		next_device->srand(next_device->autorand(0, 1023));
	}

	printf("Generating second new device...\n");

	CUniquePointer<CTrustedRandom> second_next_device(next_device->GenerateNewDevice());

	for (int idx = 0; idx < 1024; idx++) {
		printf("%d%s", second_next_device->autorand(0, 100), idx != 1023 ? ", " : "\n");
		second_next_device->srand(second_next_device->autorand(0, 1023));
	}

	next_device.reset();
	second_next_device.reset();
}

void SlowIntegerExample() {
	CSlowInteger integer = 123; //native initializer
	printf("%d %d %d %d\n", integer % 2, integer >> 36, integer << 2, integer ^ 50); //native operators
}