// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.browserservices;

import android.support.annotation.IntDef;

import org.chromium.base.metrics.RecordHistogram;
import org.chromium.base.metrics.RecordUserAction;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.util.concurrent.TimeUnit;

/**
 * Class to contain metrics recording constants and behaviour for Browser Services.
 */
public class BrowserServicesMetrics {
    @Retention(RetentionPolicy.SOURCE)
    @IntDef({VERIFICATION_RESULT_ONLINE_SUCCESS, VERIFICATION_RESULT_ONLINE_FAILURE,
            VERIFICATION_RESULT_OFFLINE_SUCCESS, VERIFICATION_RESULT_OFFLINE_FAILURE,
            VERIFICATION_RESULT_HTTPS_FAILURE, VERIFICATION_RESULT_REQUEST_FAILURE,
            VERIFICATION_RESULT_CACHED_SUCCESS})
    public @interface VerificationResultEnum {}

    // Don't reuse values or reorder values. If you add something new, change ...COUNT as well.
    public static final int VERIFICATION_RESULT_ONLINE_SUCCESS = 0;
    public static final int VERIFICATION_RESULT_ONLINE_FAILURE = 1;
    public static final int VERIFICATION_RESULT_OFFLINE_SUCCESS = 2;
    public static final int VERIFICATION_RESULT_OFFLINE_FAILURE = 3;
    public static final int VERIFICATION_RESULT_HTTPS_FAILURE = 4;
    public static final int VERIFICATION_RESULT_REQUEST_FAILURE = 5;
    public static final int VERIFICATION_RESULT_CACHED_SUCCESS = 6;
    public static final int VERIFICATION_RESULT_COUNT = 7;

    /**
     * Records the verification result for Trusted Web Activity verification.
     */
    public static void recordVerificationResult(@VerificationResultEnum int result) {
        RecordHistogram.recordEnumeratedHistogram("BrowserServices.VerificationResult", result,
                VERIFICATION_RESULT_COUNT);
    }

    /**
     * Records that a Trusted Web Activity has been opened.
     */
    public static void recordTwaOpened() {
        RecordUserAction.record("BrowserServices.TwaOpened");
    }

    /**
     * Records the time that a Trusted Web Activity has been open for.
     */
    public static void recordTwaOpenTime(long duration, TimeUnit unit) {
        RecordHistogram.recordTimesHistogram("BrowserServices.TwaOpenTime", duration, unit);
    }

    // Don't let anyone instantiate.
    private BrowserServicesMetrics() {}
}
