async function updateData(){

    const response = await fetch("/api/data");
    const data = await response.json();

    document.getElementById("humidity").innerText = data.humidity.toFixed(1);
    document.getElementById("n").innerText = data.n.toFixed(1);
    document.getElementById("p").innerText = data.p.toFixed(1);
    document.getElementById("k").innerText = data.k.toFixed(1);

}

async function pump(state){

    await fetch("/api/pump",{
        method:"POST",
        headers:{
            "Content-Type":"application/json"
        },
        body:JSON.stringify({
            state:state
        })
    });

}

setInterval(updateData,2000);