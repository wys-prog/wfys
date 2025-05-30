#pragma once

#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace wylma { namespace WylmaFileSystemEncryption {

	namespace fs = std::filesystem;

	inline uint32_t read_u32_le(std::istream& in) {
		uint8_t buf[4];
		in.read(reinterpret_cast<char*>(buf), 4);
		return (uint32_t(buf[0]) << 0) |
			   (uint32_t(buf[1]) << 8) |
			   (uint32_t(buf[2]) << 16) |
			   (uint32_t(buf[3]) << 24);
	}

	inline uint64_t read_u64_le(std::istream& in) {
		uint8_t buf[8];
		in.read(reinterpret_cast<char*>(buf), 8);
		return (uint64_t(buf[0]) << 0)  |
			   (uint64_t(buf[1]) << 8)  |
			   (uint64_t(buf[2]) << 16) |
			   (uint64_t(buf[3]) << 24) |
			   (uint64_t(buf[4]) << 32) |
			   (uint64_t(buf[5]) << 40) |
			   (uint64_t(buf[6]) << 48) |
			   (uint64_t(buf[7]) << 56);
	}

	inline void write_u32_le(std::ostream& out, uint32_t val) {
		uint8_t buf[4] = {
			static_cast<uint8_t>(val & 0xFF),
			static_cast<uint8_t>((val >> 8) & 0xFF),
			static_cast<uint8_t>((val >> 16) & 0xFF),
			static_cast<uint8_t>((val >> 24) & 0xFF)
		};
		out.write(reinterpret_cast<const char*>(buf), 4);
	}

	inline void write_u64_le(std::ostream& out, uint64_t val) {
		uint8_t buf[8] = {
			static_cast<uint8_t>(val & 0xFF),
			static_cast<uint8_t>((val >> 8) & 0xFF),
			static_cast<uint8_t>((val >> 16) & 0xFF),
			static_cast<uint8_t>((val >> 24) & 0xFF),
			static_cast<uint8_t>((val >> 32) & 0xFF),
			static_cast<uint8_t>((val >> 40) & 0xFF),
			static_cast<uint8_t>((val >> 48) & 0xFF),
			static_cast<uint8_t>((val >> 56) & 0xFF)
		};
		out.write(reinterpret_cast<const char*>(buf), 8);
	}

	int16_t save(const fs::path& root, const fs::path& output) {
		std::ofstream out(output, std::ios::binary);
		if (!out.is_open()) {
			std::cerr << "[e]: can't open output file" << std::endl;
			return -1;
		}

		for (const auto& entry : fs::recursive_directory_iterator(root)) {
			fs::path relPath = fs::relative(entry.path(), root);
			std::string relStr = relPath.generic_string(); // '/' separator
			uint32_t pathLen = static_cast<uint32_t>(relStr.size());

			write_u32_le(out, pathLen);
			out.write(relStr.c_str(), pathLen);

			char typeFlag = entry.is_directory() ? 0x00 : 0x01;
			out.write(&typeFlag, 1);

			if (entry.is_regular_file()) {
				std::ifstream file(entry.path(), std::ios::binary);
				if (!file.is_open()) {
					std::cerr << "[e]: can't open input file: " << entry.path() << std::endl;
					return -1;
				}

				uint64_t file_size = fs::file_size(entry.path());
				write_u64_le(out, file_size);

				const size_t buffer_size = 4096;
				std::vector<char> buffer(buffer_size);
				while (file) {
					file.read(buffer.data(), buffer_size);
					std::streamsize bytes_read = file.gcount();
					if (bytes_read > 0) {
						out.write(buffer.data(), bytes_read);
					}
				}
			}
		}

		return 0;
	}

	int16_t load(const fs::path& archivePath, const fs::path& outputRoot) {
		std::ifstream in(archivePath, std::ios::binary);
		if (!in.is_open()) {
			std::cerr << "[e]: can't open archive file" << std::endl;
			return -1;
		}

		while (in.peek() != EOF) {
			uint32_t pathLen = read_u32_le(in);
			if (in.eof()) break;

			std::string relPath(pathLen, '\0');
			in.read(&relPath[0], pathLen);
			if (in.eof()) break;

			fs::path fullPath = outputRoot / fs::path(relPath);

			char typeFlag;
			in.read(&typeFlag, 1);
			if (in.eof()) break;

			if (typeFlag == 0x00) {
				fs::create_directories(fullPath);
			} else if (typeFlag == 0x01) {
				fs::create_directories(fullPath.parent_path());

				uint64_t fileSize = read_u64_le(in);
				if (in.eof()) break;

				std::ofstream out(fullPath, std::ios::binary);
				if (!out.is_open()) {
					std::cerr << "[e]: can't write file: " << fullPath << std::endl;
					return -1;
				}

				const size_t bufferSize = 4096;
				std::vector<char> buffer(bufferSize);
				uint64_t remaining = fileSize;

				while (remaining > 0 && in) {
					std::streamsize toRead = static_cast<std::streamsize>(std::min<uint64_t>(bufferSize, remaining));
					in.read(buffer.data(), toRead);
					std::streamsize bytesRead = in.gcount();
					if (bytesRead > 0) {
						out.write(buffer.data(), bytesRead);
						remaining -= bytesRead;
					} else {
						std::cerr << "[e]: unexpected EOF while reading file contents.\n";
						return -1;
					}
				}
			} else {
				std::cerr << "[e]: invalid entry type flag: " << static_cast<int>(typeFlag) << std::endl;
				return -1;
			}
		}

		return 0;
	}


}}
