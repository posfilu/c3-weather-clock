idf_component_register(
    SRCS "main.c"
    PRIV_REQUIRES esp_driver_gpio spi_flash esp_app_format esp_system
)
