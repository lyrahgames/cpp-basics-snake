#include <application.hpp>
#include <string>

int main(int argc, char** argv) {
  using namespace std;

  const string exe_path = argv[0];
  const auto pos = exe_path.find_last_of('/');
  const auto exe = exe_path.substr(pos + 1);
  const auto exe_dir = exe_path.substr(0, pos + 1);

  application app{500, 500, exe_dir};
  app.execute();
}