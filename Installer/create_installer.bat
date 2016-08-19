candle Product.wxs -ext WixUtilExtension -ext WixUIExtension -out Product.wixobj

light.exe Product.wixobj -spdb -ext WixUtilExtension -ext WixUIExtension -out "R&S PA Compression Test 2.2.msi"