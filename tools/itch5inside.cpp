#include <jb/itch5/compute_inside.hpp>
#include <jb/itch5/process_iostream.hpp>
#include <jb/fileio.hpp>
#include <jb/log.hpp>

#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace {

class config : public jb::config_object {
 public:
  config();
  config_object_constructors(config);

  void validate() const override;

  jb::config_attribute<config,std::string> input_file;
  jb::config_attribute<config,std::string> output_file;
  jb::config_attribute<config,jb::log::config> log;
};


} // anonymous namespace

int main(int argc, char* argv[]) try {
  config cfg;
  cfg.load_overrides(
      argc, argv, std::string("itch5_inside.yaml"), "JB_ROOT");
  jb::log::init(cfg.log());

  boost::iostreams::filtering_istream in;
  jb::open_input_file(in, cfg.input_file());

  boost::iostreams::filtering_ostream out;
  jb::open_output_file(out, cfg.output_file());

  auto cb = [&out](jb::itch5::compute_inside::time_point recv_ts,
               jb::itch5::message_header const& header,
               jb::itch5::stock_t const& stock,
               jb::itch5::half_quote const& bid,
               jb::itch5::half_quote const& offer) {
    out << header.timestamp.ts.count()
        << " " << header.stock_locate
        << " " << stock
        << " " << bid.first.as_integer()
        << " " << bid.second
        << " " << offer.first.as_integer()
        << " " << offer.second
        << "\n";
  };

  jb::itch5::compute_inside handler(cb);
  jb::itch5::process_iostream(in, handler);

  return 0;
} catch(jb::usage const& u) {
  std::cerr << u.what() << std::endl;
  return u.exit_status();
} catch(std::exception const& ex) {
  std::cerr << "Standard exception raised: " << ex.what() << std::endl;
  return 1;
} catch(...) {
  std::cerr << "Unknown exception raised" << std::endl;
  return 1;
}

namespace {
config::config()
    : input_file(desc("input-file").help(
        "An input file with ITCH-5.0 messages."), this)
    , output_file(desc("output-file").help(
        "The name of the file where to store the inside data."
        "  Files ending in .gz are automatically compressed."), this)
    , log(desc("log"), this)
{}

void config::validate() const {
  if (input_file() == "") {
    throw jb::usage(
        "Missing input-file setting."
        "  You must specify an input file.", 1);
  }
  if (output_file() == "") {
    throw jb::usage(
        "Missing output-file setting."
        "  You must specify an output file.", 1);
  }
  log().validate();
}

} // anonymous namespace
