//
//! Copyright 2020 Alibaba Group Holding Limited.
//!
//! Licensed under the Apache License, Version 2.0 (the "License");
//! you may not use this file except in compliance with the License.
//! You may obtain a copy of the License at
//!
//! http://www.apache.org/licenses/LICENSE-2.0
//!
//! Unless required by applicable law or agreed to in writing, software
//! distributed under the License is distributed on an "AS IS" BASIS,
//! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//! See the License for the specific language governing permissions and
//! limitations under the License.

use crate::generated::gremlin as pb;
use crate::process::traversal::traverser::Traverser;
use bit_set::BitSet;
use dyn_type::Object;
use pegasus::api::function::*;

impl MapFunction<Traverser, Traverser> for pb::PathStep {
    fn exec(&self, input: Traverser) -> FnResult<Traverser> {
        let path = input.take_path();
        Ok(Traverser::Object(Object::DynOwned(Box::new(path))))
    }
}

pub struct PathLocalCountStep {
    pub tags: BitSet,
    pub remove_tags: BitSet,
}

impl MapFunction<Traverser, Traverser> for PathLocalCountStep {
    fn exec(&self, mut input: Traverser) -> FnResult<Traverser> {
        let count = input.get_path_len() as i64;
        input.split_with_value(count, &self.tags);
        input.remove_tags(&self.remove_tags);
        Ok(input)
    }
}
