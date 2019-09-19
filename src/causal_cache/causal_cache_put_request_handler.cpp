//  Copyright 2019 U.C. Berkeley RISE Lab
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "causal/causal_cache_utils.hpp"

void put_request_handler(const string &serialized, StoreType &unmerged_store,
                         StoreType &causal_cut_store,
                         VersionStoreType &version_store,
                         map<string, Address> &request_id_to_address_map,
                         KvsClientInterface *client, logger log) {
  CausalRequest request;
  request.ParseFromString(serialized);
  for (CausalTuple tuple : request.tuples()) {
    Key key = tuple.key();
    auto lattice = std::make_shared<MultiKeyCausalLattice<SetLattice<string>>>(
        to_multi_key_causal_payload(
            deserialize_multi_key_causal(tuple.payload())));
    // write to KVS
    string req_id = client->put_async(key, serialize(*lattice),
                                      LatticeType::MULTI_CAUSAL);

    request_id_to_address_map[req_id] = request.response_address();
  }
}
