// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views_mode_controller.h"

#include "base/feature_list.h"
#include "ui/base/ui_base_features.h"

#if defined(OS_MACOSX) && BUILDFLAG(MAC_VIEWS_BROWSER)

namespace views_mode_controller {

bool IsViewsBrowserCocoa() {
  return features::IsViewsBrowserCocoa() &&
         !base::FeatureList::IsEnabled(features::kExperimentalUi);
}

}  // namespace views_mode_controller

#endif
