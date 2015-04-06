#ifndef KALLISTO_H5_UTILS
#define KALLISTO_H5_UTILS

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "hdf5.h"

// XXX: remember to cleanup result!
const char** vec_to_ptr(const std::vector<std::string>& v);

const double* vec_to_ptr(const std::vector<double>& v);

const int* vec_to_ptr(const std::vector<int>& v);

hid_t get_datatype_id(const std::vector<std::string>& v);

hid_t get_datatype_id(const std::vector<double>& v);

hid_t get_datatype_id(const std::vector<int>& v);

// str_vec: a vector of string to be written out
// group_id: a group_id which has already been opened
// dataset_name: the to write out to
// release_type: if 'true', release the datatype and ptr
// compression_level: the level of compression (6 seems reasonable)
//
// return: the status of H5Dwrite (last H5 operation)
template <typename T>
herr_t vector_to_h5(
    const std::vector<T>& str_vec,
    hid_t group_id,
    const std::string& dataset_name,
    bool release_type,
    uint compression_level = 6
    ) {
  herr_t status;

  hsize_t dims[1] = {str_vec.size()};

  // create the propery which allows for compression
  hid_t prop_id = H5Pcreate(H5P_DATASET_CREATE);
  // chunk size is same size as vector
  status = H5Pset_chunk(prop_id, 1, dims);
  status = H5Pset_deflate(prop_id, compression_level);

  // create the data type
  hid_t datatype_id = get_datatype_id(str_vec);

  // create the dataspace
  hid_t dataspace_id = H5Screate_simple(1, dims, NULL);

  // create the dataset
  hid_t dataset_id = H5Dcreate(group_id, dataset_name.c_str(), datatype_id,
      dataspace_id, H5P_DEFAULT, prop_id, H5P_DEFAULT);

  // get the ptrs from the string and write out
  auto ptr = vec_to_ptr(str_vec);
  status = H5Dwrite(dataset_id, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT,
      ptr);

  status = H5Pclose(prop_id);
  status = H5Dclose(dataset_id);
  status = H5Sclose(dataspace_id);
  if (release_type) {
    status = H5Tclose(datatype_id);
    delete [] ptr;
  }

  return status;
}

#endif // KALLISTO_H5_UTILS
