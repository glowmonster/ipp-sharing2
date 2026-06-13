use crate::config::DeviceConfig;
use log::{error, info};
use mdns_sd::{ServiceDaemon, ServiceInfo};
use std::thread;
use std::time::Duration;

pub fn serve_dnssd(device_config: &DeviceConfig, port: u16, type_name: &str) {
    let device_config = device_config.clone();
    let type_name = type_name.to_string();
    thread::spawn(move || {
        if let Err(e) = serve_dnssd_thread(&device_config, port, type_name.as_str()) {
            error!("Failed to serve DNS-SD for {}: {}", device_config.name, e);
        }
    });
}

fn serve_dnssd_thread(
    device_config: &DeviceConfig,
    port: u16,
    type_name: &str,
) -> anyhow::Result<()> {
    let service_type = format!("_{}._{}.local.", type_name, "tcp");
    let host_name = format!("{}.local.", device_config.name);

    // Build TXT record properties
    let mut properties: Vec<(&str, &str)> = Vec::new();
    properties.push(("txtvers", "1"));
    properties.push(("qtotal", "1"));
    properties.push((
        "rp",
        device_config
            .basepath
            .as_str()
            .strip_prefix('/')
            .unwrap_or(device_config.basepath.as_str()),
    ));
    properties.push(("ty", device_config.make_and_model.as_str()));
    properties.push(("priority", "0"));
    let mut pdl = Vec::<&str>::new();
    if cfg!(any(feature = "winpdf", feature = "pdfium")) {
        pdl.push("application/pdf");
    }
    pdl.push("application/vnd.ms-xpsdocument");
    pdl.push("image/pwg-raster");
    pdl.push("image/urf");
    properties.push(("pdl", pdl.join(",").leak()));
    properties.push(("note", ""));
    let uuid_str = device_config.uuid.hyphenated().to_string();
    properties.push(("UUID", Box::leak(uuid_str.into_boxed_str())));

    let service_info = ServiceInfo::new(
        &service_type,
        &device_config.name,
        &host_name,
        "", // empty IP for auto-resolve
        port,
        properties.as_slice(),
    )?;

    let daemon = ServiceDaemon::new()?;
    daemon.register(service_info)?;
    info!("DNS-SD registered for {} {}", type_name, device_config.name);

    // Keep the daemon alive forever
    loop {
        thread::sleep(Duration::from_secs(3600));
    }
}
