// begin example.
namespace foo { class bar; class baz;};
class foo::bar : public std::exception {};
class foo::baz : public std::exception { int f(); };
class Test : public test {};
// end example.

class a
{
	int a1() throw();
	int b1() throw(int, float);

	class b
	{
		int b1();
		int B1();
	}
}

class b
{
	int a1();
	int b1();
}

class c
{
	int b1();
}


namespace std
{

class A { A(); };
class B { B(); };

class JInPipeStream1 : public istream
{
public:

	JInPipeStream(const int fd, const JBoolean close);

	virtual ~JInPipeStream();

	void	close();

	JBoolean	WillClosePipe() const;
	void		ShouldClosePipe(const JBoolean close = kJTrue);

private:

	stdiobuf	itsBuffer;
	const int	itsDescriptor;
	JBoolean	itsCloseFlag;	// kJTrue => close when we are destructed

private:

	// not allowed

	JInPipeStream(const JInPipeStream& source);
	const JInPipeStream& operator=(const JInPipeStream& source);
};

};
