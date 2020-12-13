
#include <utility>
#include <vector>
#include <string>

#include "SpiritSA.h"

struct A
{
  int64_t a, b, c;
  std::variant<int64_t, double> var;
  std::optional<int64_t> opt;
  std::vector<int64_t> vec;
};
struct B
{
  std::variant<int64_t, double> var;
  std::optional<recursive_wrapper<B>> b;
};
struct XmlNode
{
  std::string tag_name;
  std::vector<std::variant<std::string, XmlNode>> children;
};

void print_xml(const XmlNode& n, std::size_t indent = 0)
{
  auto ind = [&]() { std::string r; for (std::size_t i = 0; i < indent; i++) r += "  "; return r; };
  std::cout << ind() << "<" << n.tag_name << ">\n";
  for (const auto& c : n.children)
  {
    if (c.index() == 0)
    {
      std::cout << ind() << "  " << std::get<std::string>(c) << "\n";
    }
    else
    {
      print_xml(std::get<XmlNode>(c), indent + 1);
    }
  }
  std::cout << ind() << "</" << n.tag_name << ">\n";
}

std::string cb(std::tuple<std::string&, std::vector<std::variant<std::string, XmlNode>>&> t)
{
  return std::get<0>(t);
}

struct G_Signal
{
    std::string name;
    std::optional<std::string> multiplexer_indicator;
    uint64_t start_bit;
    uint64_t signal_size;
    char byte_order;
    char value_type;
    //double factor;
    //double offset;
    //double minimum;
    //double maximum;
    //std::string unit;
    //std::vector<std::string> receivers;
};


int main()
{
  XmlNode n;
  std::string test = "<asdf><inner>text</inner><inner><inner2></inner2></inner></asdf>";
  rule<const char*, XmlNode> xml_node;
  rule<const char*, std::string> text;
  text = +(char_ - lit("<"));
  auto text2 = +(char_ - lit("<") - lit(":"));
  xml_node %= lit("<") >> !lit("/") > +(char_ - lit(">")) > lit(">") > *(text2 | xml_node) > lit("</") > lit(cb) > lit(">");
  const auto* beg = &test[0];
  xml_node.parse(beg, beg + test.size(), n);
  print_xml(n);

  auto C_Identifier = *(char_ - lit(" "));
  rule<const char*, G_Signal> dbc_signal;
  test = "SG_ s3_6 m3 : 49|1@1+ (1,0) [0|0] "" Receiver";
  dbc_signal %= lit("SG_ ") >> C_Identifier > lit(" ") > -C_Identifier > lit(" : ") > int_ > lit("|") > int_ > lit("@") > char_ > char_;
  G_Signal gsig;
  beg = &test[0];
  dbc_signal.parse(beg, beg + test.size(), gsig);
  std::cout << "break\n";

  //auto rt = lit("asdf") - int_;

  //std::string test = "asdf 1 2 3 4 5 6 7 8 9 1 asdf";
  //A a;
  //rule<const char*, A> r;
  //auto ir = lit("asdf") > int_ > int_ > int_ > (int_ | double_) > -int_ > *int_ > lit(cb);
  //r %= ir;
  //r.parse(&test[0], &test[0] + test.size(), a);

  //test = "5 5";
  //std::variant<int64_t, double> var;
  //B b;
  //rule<const char*, B> r2;
  //auto ir2 = (int_ | double_);
  //auto ir3 = ir2 > -r2;
  //r2 %= ir3;
  //r2.parse(&test[0], &test[0] + test.size(), b);
}
