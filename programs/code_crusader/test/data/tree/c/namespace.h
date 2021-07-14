namespace Test1
{
	class /*comment*/ Foo {};

	namespace/*comment*/Test2
	{
		class Bar : /*comment {*/ private Foo
		{
			friend class Foo;
			public:
			template <class T> class Zod {};
		};

		class Zot : // comment {
			public Bar::Zod<int> { "namespace foo {"; };

		namespace
		{
			class Baz : protected Bar, // comment
				public Shug {};
		}
	}
};
