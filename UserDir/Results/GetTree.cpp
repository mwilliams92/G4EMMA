#include <iostream>
#include <fstream>
#include <string>
#include "TFile.h"
#include "TTree.h"

void create_tree(const char* input_filename, const char* output_filename) {
    // Open the input file
    std::ifstream infile(input_filename);
    if (!infile.is_open()) {
        std::cerr << "Error opening input file: " << input_filename << std::endl;
        return;
    }

    // Create a new ROOT file
    TFile *file = new TFile(output_filename, "RECREATE");
    if (!file->IsOpen()) {
        std::cerr << "Error creating output file: " << output_filename << std::endl;
        return;
    }

    // Create a TTree
    TTree *tree = new TTree("tree", "A tree with four branches");

    // Variables to hold the data
    float col1, col2, col3, col4;

    // Create branches
    tree->Branch("col1", &col1, "col1/F");
    tree->Branch("col2", &col2, "col2/F");
    tree->Branch("col3", &col3, "col3/F");
    tree->Branch("col4", &col4, "col4/F");

    // Read the input file line by line
    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        if (!(iss >> col1 >> col2 >> col3 >> col4)) {
            std::cerr << "Error reading line: " << line << std::endl;
            continue; // Skip the current line if there was an error
        }
        tree->Fill(); // Fill the tree with the current line's data
    }

    // Write the tree to the ROOT file
    tree->Write();

    // Close the files
    infile.close();
    file->Close();

    std::cout << "Tree successfully created and saved to " << output_filename << std::endl;
}
