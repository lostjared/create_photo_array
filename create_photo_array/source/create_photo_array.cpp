
#include<opencv2/opencv.hpp>
#include<iostream>
#include<string>
#include<cctype>
#include<cstdio>
#include<dirent.h>
#include<sys/stat.h>
#include<fstream>
#include<sstream>
#include<unistd.h>
#include<vector>
#include<algorithm>

std::string prefix_string = "http://lostsidedead.biz/filtered/";
bool output_thumbnail = true;

std::string string_to_lower(const std::string &text) {
    std::string temp;
    for(int i = 0; i < text.length(); ++i) {
        temp += tolower(text[i]);
    }
    return temp;
}

cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor)
{
    cv::Mat output;
    double h1 = dstSize.width * (input.rows/(double)input.cols);
    double w2 = dstSize.height * (input.cols/(double)input.rows);
    if( h1 <= dstSize.height) {
        cv::resize( input, output, cv::Size(dstSize.width, h1));
    } else {
        cv::resize( input, output, cv::Size(w2, dstSize.height));
    }
    int top = (dstSize.height-output.rows) / 2;
    int down = (dstSize.height-output.rows+1) / 2;
    int left = (dstSize.width - output.cols) / 2;
    int right = (dstSize.width - output.cols+1) / 2;
    cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor );
    return output;
}

void add_directory(std::string path, std::string file_type, std::vector<std::string> &files) {
    DIR *dir = opendir(path.c_str());
    if(dir == NULL) {
        std::cerr << "Error could not open directory: " << path << "\n";
        return;
    }
    dirent *file_info;
    while( (file_info = readdir(dir)) != 0 ) {
        std::string f_info = file_info->d_name;
        
        if(f_info.find("thumbnail") != std::string::npos)
            continue;
        
        if(f_info == "." || f_info == "..")  continue;
        std::string fullpath=path+"/"+f_info;
        struct stat s;
#ifdef WIN32
        stat(fullpath.c_str(), &s);
#else
        lstat(fullpath.c_str(), &s);
#endif
        if(S_ISDIR(s.st_mode)) {
            if(f_info.length()>0 && f_info[0] != '.')
                add_directory(path+"/"+f_info, file_type, files);
            continue;
        }
        if(f_info.length()>0 && f_info[0] != '.') {
            std::string ext;
            auto pos = f_info.rfind(".");
            if(pos != std::string::npos) {
                std::string ext = f_info.substr(pos+1, f_info.length()-pos);
                std::string filename = f_info.substr(0, pos);
                if(string_to_lower(ext) == string_to_lower(file_type)) {
                    std::cout << fullpath << "\n";
                    files.push_back(fullpath);
                    continue;
                }
            }
        }
    } closedir(dir);
}


void write_page(std::fstream &file, std::vector<std::string> &vf, int page_num, int start, int max) {
    static int counter = 0;
    for(int i = start; i < max && i < vf.size(); ++i) {
        std::string filename = vf[i];
        std::string thumb_name = filename.substr(filename.rfind("/")+1, filename.length());
        if(output_thumbnail == true) {
            cv::Mat fname = cv::imread(filename);
            cv::Mat new_frame = resizeKeepAspectRatio(fname, cv::Size(320, 180), cv::Scalar(255,255,255));
            std::stringstream s;
            s << "thumbnail/thumbnail." << thumb_name;
            cv::imwrite(s.str(), new_frame);
        }
        if(filename[0] == '.' && filename[1] == '/')
            filename = filename.substr(2, filename.length());
        file << "<a href=\"" << prefix_string << filename << "\"><img src=\"" << prefix_string << "thumbnail/thumbnail." << thumb_name << "\"></a>\n";
        ++counter;
        if((counter%5)==0)
            file << "<br>\n\n";
        if(output_thumbnail == true) std::cout << "Wrote: " << filename << " thumbnail: thumbnail." << thumb_name << "\n";
    }
}

int main(int argc, char **argv) {
    if(argc == 2 || argc == 3 || argc == 4) {
        if(argc == 3)
            prefix_string = argv[2];
        if(argc == 4 && std::string(argv[3]) == "nothumb") {
            output_thumbnail = false;
            prefix_string = argv[2];
        }
        std::vector<std::string> found_files;
        add_directory(argv[1], "png", found_files);
        if(found_files.size()==0) {
            std::cout << "create_photo_array: found zero files...\n";
            exit(EXIT_SUCCESS);
        }
        std::sort(found_files.begin(), found_files.end());
        const int dir_err = mkdir("thumbnail", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(dir_err == -1) {
            std::cerr << "Error could not create thumbnail directory...\n";
        }
        int value_offset = 0;
        
        for(int i = 0; i < (found_files.size()/200)+1; ++i) {
            std::fstream file;
            std::ostringstream stream;
            stream << "thumbnail_page-" << std::setfill('0') << std::setw(5) << i << ".html";
            file.open(stream.str(), std::ios::out);
            if(!file.is_open()) {
                std::cerr << "Could not open file: " << stream.str() << " failure...\n";
                exit(EXIT_FAILURE);
            }
            file << "<!DOCTYPE html><html><head><title>Acid Cam Photos Page " << i << "</title></head>\n";
            file << "<body>\n";
            write_page(file, found_files, i, value_offset, value_offset+200);
            stream.str("");
            stream << prefix_string << "thumbnail_page-" << std::setfill('0') << std::setw(5) << i+1 << ".html";
            if(i < (found_files.size()/200))
                file << "\n<br><br><br><a href=\"" << stream.str() << "\">Next Page</a><br><br>\n\n";
            file << "\n</body></html>\n";
            file.close();
            value_offset += 200;
        }
        std::cout << "complete...\n";
    } else {
        std::cout << argv[0] << ": Use with arguments\n";
        std::cout << "create_photo_array path\n" << "create_photo_array path prefix\n" << "create_photo_array path prefix nothumb\n";
    }
    return 0;
}
