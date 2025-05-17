
idf_component_register(SRCS "https_ota.c" "ble_helper/bluedroid_gatts.c" "ble_helper/nimble_gatts.c"
                    "ble_helper/ble_api.c" "cert.c"
                    INCLUDE_DIRS "." "./ble_helper/include/"
                    
                    # Embed the server root certificate into the final binary
                    #  EMBED_TXTFILES ${project_dir}/cert/cert.pem
)           