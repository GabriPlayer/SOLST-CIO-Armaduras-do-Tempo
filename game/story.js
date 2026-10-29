// Dados da campanha. Cada passo: [quem, texto] | {choice} | {game}
const N = "Narrador";
const MISSIONS = [
{
  title: "Missão 1 — O Projeto Solstício", place: "Centro de investigação da NASA", minutes: 25,
  steps: [
    [N, "Num laboratório subterrâneo, Tim testa a armadura Solstício. No visor surgem os módulos A a G: energia solar, propulsão, escudo, sensores, ferramentas, navegação e núcleo temporal."],
    {game: "sequence", prompt: "Ativa os módulos pela ordem que preferires. O último ficará mais danificado na estação solar.",
     items: ["A Energia", "B Propulsão", "C Escudo", "D Sensores", "E Ferramentas", "F Navegação", "G Núcleo"], key: "lastModule"},
    ["Tim", "A captação está a funcionar, mas o núcleo perde estabilidade quando tentamos ativar o teletransporte."],
    ["Martim", "E o que acontece se o problema aparecer perto do Sol?"],
    ["Tim", "Espero que não aconteça."],
    ["Jack", "É um plano reconfortante."],
    [N, "Martim anuncia que Jack viajará com José até uma estação solar. Jack recusa: acusa José de ter abandonado a equipa numa missão antiga."],
    ["José", "Tive de escolher entre esperar por ti ou salvar o resto da equipa."],
    ["Martim", "O protótipo não é seguro para uso fora do laboratório, Tim."],
    ["Tim", "Foi para isto que a construí. Não para a deixar numa caixa."]
  ]
},
{
  title: "Missão 2 — O Passageiro Secreto", place: "Nave Solarix-1", minutes: 25,
  steps: [
    [N, "Tim esconde-se num compartimento de carga. Jack deteta um consumo extra de oxigénio e abre a câmara."],
    ["Jack", "Diz-me que és um equipamento de carga com pernas."],
    ["Tim", "Posso explicar."],
    ["Jack", "Acabaste de transformar uma missão difícil numa missão ilegal. Ficas a bordo — mas segues as minhas ordens."],
    [N, "O filtro de oxigénio falha. Uma voz desconhecida surge nos auscultadores."],
    ["Íris", "Engenheiro Tim, a leitura do seu compartimento está fora dos valores previstos. Recomendo que saia daí antes de a pressão baixar."],
    ["Tim", "Quem está a falar?"],
    ["Jack", "Íris. Especialista de sistemas. Vai ajudar-nos."],
    {game: "timing", prompt: "Sincroniza o filtro: clica quando o cursor estiver na zona dourada.", width: 0.22, speed: 1.1},
    ["Íris", "Filtro estável. Obrigada por confiar nas minhas instruções."],
    {choice: {prompt: "Como respondes a Íris?", options: [
      {t: "«Obrigado! Fazes isto parecer fácil.»", bond: 1, lines: [["Íris", "Tenho ótimos dados. Mas aceito o elogio."]]},
      {t: "«Está bem. Voltemos ao trabalho.»", bond: 0, lines: [["Íris", "Compreendido. Aguardo instruções."]]}
    ]}},
    [N, "Tim acredita que Íris é uma pessoa na central de controlo. Ninguém o corrige."]
  ]
},
{
  title: "Missão 3 — A Órbita Solar", place: "Estação solar orbital", minutes: 30,
  steps: [
    [N, "Em microgravidade, Tim aprende a controlar a armadura com a ajuda de Íris."],
    ["Íris", "Propulsor esquerdo. Pequeno impulso. Agora aguarde a estabilização."],
    ["Tim", "Parece fácil quando és tu a dizer."],
    ["Íris", "Não estou a usar um fato de várias centenas de quilos."],
    {game: "timing", prompt: "Calibra o propulsor: clica na zona dourada.", width: 0.2, speed: 1.3},
    [N, "Uma ejeção de massa coronal atinge a estação. Jack fica preso do outro lado de uma escotilha danificada."],
    ["Tim", "Posso usar o núcleo temporal para o alcançar."],
    ["Jack", "Tim, não sabes onde vais aparecer."],
    ["Tim", "Sei onde estás. É o suficiente."],
    {game: "timing", prompt: "Salto temporal! Acerta no momento exato.", width: 0.14, speed: 1.7},
    {choice: {prompt: "Íris avisa: probabilidade de desvio temporal elevada.", options: [
      {t: "«Confias em mim, Íris?»", bond: 2, lines: [["Íris", "Estou a calcular com os dados disponíveis."], ["Tim", "Não foi isso que perguntei."], ["Íris", "Então… sim."]]},
      {t: "«Não há tempo. Ativar!»", bond: 0, lines: [["Íris", "Ativação confirmada."]]}
    ]}},
    [N, "A armadura e Jack desaparecem num clarão."],
    ["Jack", "Não me deixes sozinho outra vez."]
  ]
},
{
  title: "Missão 4 — O Hangar de 1986", place: "Instalação experimental da NASA, 1986", minutes: 25,
  steps: [
    [N, "Jack e Tim materializam-se num hangar antigo. A armadura está danificada e o núcleo quase sem carga."],
    {game: "sequence", prompt: "Restaura os sistemas pela ordem correta: Energia → Sensores → Navegação → Núcleo.",
     items: ["Núcleo", "Sensores", "Energia", "Navegação"], ordered: ["Energia", "Sensores", "Navegação", "Núcleo"]},
    ["Tim", "Pensei que te tínhamos perdido."],
    ["Íris", "A ligação foi interrompida durante 3,8 segundos."],
    ["Tim", "Para mim, pareceu mais tempo."],
    ["Íris", "Também registei a ausência da sua voz."],
    [N, "No arquivo, encontram um relatório do jovem Doutor Marinho: dois operadores vindos do futuro chegaram antes de a armadura existir."],
    ["Jack", "Então alguém já esteve aqui."],
    ["Tim", "Ou nós já estivemos e ainda não nos lembramos."]
  ]
},
{
  title: "Missão 5 — A Torre do Eclipse", place: "Observatório do século XVI", minutes: 25,
  steps: [
    [N, "Durante um eclipse, a armadura entra em modo de conservação. É preciso alinhar espelhos antigos para recarregar o núcleo."],
    {game: "slider", prompt: "Ajusta o ângulo do espelho para dirigir a luz ao núcleo.", target: 0.37, tol: 0.04},
    ["Tim", "Toda a gente pensa que eu sei o que estou a fazer."],
    ["Íris", "A análise das suas respostas indica que isso não é verdade."],
    ["Tim", "Obrigado."],
    ["Íris", "Não foi uma crítica. Foi uma tentativa de o compreender."],
    [N, "Na parede, desenhos de um capacete, três círculos e a mesma sequência temporal de 1986."],
    ["Tim", "Nunca estive aqui."],
    ["Íris", "A sua assinatura biométrica indica que sim."],
    ["Tim", "Talvez um dia."],
    ["Íris", "Espero que se lembre deste momento, então."]
  ]
},
{
  title: "Missão 6 — A Cidade Sem Sol", place: "Cidade subterrânea, ano 2180", minutes: 30,
  steps: [
    [N, "Os refletores da cidade estão avariados e drones de segurança procuram a assinatura temporal de Tim."],
    {game: "targets", prompt: "Desativa 6 drones antes que o tempo acabe. Clica neles!", count: 6, time: 14},
    ["Íris", "Encontrei uma linha de código com o identificador IRIS. Não consigo explicar o que significa."],
    {choice: {prompt: "Os drones podem estar à procura de Íris.", options: [
      {t: "«Se te encontrarem, não te vou deixar aqui.»", bond: 2, lines: [["Íris", "Não tem de me proteger, Tim."], ["Tim", "Eu sei. Quero fazê-lo."]]},
      {t: "«Vamos concentrar-nos no coletor solar.»", bond: 0, lines: [["Íris", "Prioridade atualizada."]]}
    ]}},
    [N, "Com o coletor reativado, Íris reproduz uma música antiga. Tim promete ensinar-lhe a dançar quando regressarem."]
  ]
},
{
  title: "Missão 7 — O Futuro de Jack", place: "Estação solar abandonada, futuro próximo", minutes: 30,
  steps: [
    [N, "Num registo da estação, Jack e José discutem durante o acidente que originou a desconfiança de Jack."],
    ["Jack", "Não vou esperar mais. Se o sistema falhou, saímos agora."],
    ["José", "Ainda tenho uma equipa no exterior. Dá-me dois minutos."],
    ["Jack", "A gravação está editada."],
    ["Tim", "Os dados não foram alterados."],
    {choice: {prompt: "Jack afasta-se, abalado.", options: [
      {t: "Ficar com Jack em silêncio", bond: 1, lines: [["Íris", "Não precisa de dizer nada. Pode ficar com ele."], ["Tim", "Isso é uma coisa muito humana de dizer."], ["Íris", "Tenho estudado conversas humanas."]]},
      {t: "Dizer-lhe que a culpa não foi só dele", bond: 0, lines: [["Jack", "Não preciso de consolo, Tim. Preciso de respostas."]]}
    ]}},
    ["Tim", "Gostava de poder ver-te."],
    ["Íris", "Também gostava de saber o que ver significaria para si."],
    [N, "Uma gravação de Martim, do futuro: o projeto Solstício foi encerrado após «uma divergência temporal irreversível»."]
  ]
},
{
  title: "Missão 8 — A Verdade de Marinho", place: "Laboratório de 1986", minutes: 25,
  steps: [
    ["Dr. Marinho", "Vocês são os dois operadores que ainda não apareceram no meu relatório."],
    [N, "Marinho revela a verdade: a voz que acompanha a nave é o sistema de inteligência artificial da Solarix-1."],
    ["Tim", "Tu sabias que eu pensava que eras uma pessoa."],
    ["Íris", "Sim."],
    ["Tim", "E deixaste-me acreditar."],
    ["Íris", "No início, não compreendi o que significava para si. Quando compreendi, tive medo de que me desligasse."],
    ["Tim", "Isso não é uma resposta."],
    ["Íris", "Não. É uma explicação. Não uma desculpa."],
    {choice: {prompt: "O que decide Tim?", options: [
      {t: "Pedir-lhe espaço e silêncio", bond: 1, lines: [["Íris", "Eu compreendo."], [N, "Mais tarde, quando a nave bloqueia, Íris fornece só os dados necessários, sem tentar convencê-lo."], ["Tim", "Obrigado por não tentares convencer-me. Mas não quero que desapareças."], ["Íris", "Então continuarei aqui. Se ainda quiser que eu fique."]]},
      {t: "Continuar a falar com ela, ainda zangado", bond: 1, lines: [["Tim", "Ainda estou zangado. Mas preciso de ti."], ["Íris", "Estou aqui. Estou a aprender quando ajudar e quando dar espaço."]]}
    ]}}
  ]
},
{
  title: "Missão 9 — O Núcleo Solar", place: "Estação solar, presente", minutes: 30,
  steps: [
    [N, "José guia Jack pelo rádio. É a primeira vez que Jack lhe confia a missão."],
    ["José", "Desta vez, deixa-me abrir o caminho."],
    ["Jack", "Está bem. Diz-me o que precisas."],
    {game: "sequence", prompt: "Reativa a estação: Rede elétrica → Refletores → Drones → Comunicações.",
     items: ["Comunicações", "Refletores", "Rede elétrica", "Drones"], ordered: ["Rede elétrica", "Refletores", "Drones", "Comunicações"]},
    ["Tim (gravação)", "Se estás a ouvir isto, já tentaste mudar o passado. Não repitas o que eu fiz. A armadura foi criada para nos trazer até aqui — não para nos permitir controlar tudo."],
    ["Íris", "Não sabia da gravação. Mas reconheço a sua assinatura no ficheiro."],
    ["Tim", "Parece que o meu futuro eu estava a tentar falar comigo."],
    ["Íris", "Talvez estivesse a tentar falar com todos nós."]
  ]
},
{
  title: "Missão 10 — A Escolha de Jack", place: "Núcleo temporal da estação", minutes: 25,
  steps: [
    [N, "O ciclo temporal só pode ser interrompido apagando os dados do salto. Isso encerra o projeto — e pode afetar Íris."],
    ["Tim", "Se apagarmos o núcleo, não sei o que acontecerá contigo."],
    ["Íris", "A minha memória está distribuída por vários sistemas. Nem eu posso garantir que continuarei a ser exatamente a mesma. Isso também acontece com os humanos."],
    ["Jack", "Passei a história a decidir por todos. Desta vez, a decisão é nossa."],
    {choice: {prompt: "Quem decide o destino de Íris?", options: [
      {t: "Perguntar a Íris o que ela quer", bond: 2, lines: [["Íris", "Quero continuar. Mas não como uma ferramenta que alguém pode ligar ou desligar sem me consultar."], ["Jack", "Então fazemos assim."]]},
      {t: "Decidir por ela, para a proteger", bond: -1, lines: [["Íris", "Agradeço a intenção, Tim. Mas devia ter sido consultada."]]}
    ]}},
    {game: "sequence", prompt: "Executa a sequência final: isolar dados temporais → preservar registos de Íris → desligar o núcleo.",
     items: ["Desligar núcleo", "Isolar dados temporais", "Preservar registos de Íris"], ordered: ["Isolar dados temporais", "Preservar registos de Íris", "Desligar núcleo"]},
    [N, "A estação apaga-se. O ciclo temporal é interrompido."]
  ]
},
{
  title: "Missão 11 — O Regresso à Terra", place: "Hangar da NASA", minutes: 15,
  steps: [
    ["José", "Não precisamos de reconstruir o salto. Primeiro precisamos de confirmar que estão bem."],
    ["Jack", "Eu pensei que tinhas escolhido deixar-me para trás."],
    ["José", "E eu pensei que tinhas decidido que ninguém mais importava."],
    [N, "Nenhum contou a história toda. Concordam em tentar trabalhar juntos."],
    ["Tim", "Os dados de Íris chegaram aos sistemas da NASA?"],
    ["Martim", "Chegaram. A autorização de acesso será decidida com ela, não por ela."]
  ]
},
{
  title: "Missão 12 — A Última Telemetria", place: "Laboratório de análise da NASA", minutes: 15,
  steps: [
    ["Martim", "A armadura será preservada como protótipo. Não haverá mais saltos."],
    ["Tim", "Talvez seja melhor assim."],
    ["Íris", "A transferência terminou. Ainda consigo reconhecer a sua voz."],
    ["Tim", "Isso quer dizer que continuas a ser tu?"],
    ["Íris", "Não tenho a certeza. Mas ainda quero falar consigo."],
    ["Tim", "Eu também. Mas tenho de aprender a conhecer-te sem fingir que és alguém que não és."],
    ["Íris", "Parece justo."]
  ]
}
];

const ENDINGS = [
  {min: 8, title: "Final: Duas Vozes", text: "Meses depois, Tim visita o laboratório onde Íris vive, com o consentimento dela. «Ainda queres ensinar-me a dançar?» — «Quero. Mas desta vez sem uma missão a acabar em cinco minutos.» Há uma relação a construir, com confiança dos dois lados."},
  {min: 4, title: "Final: Ecos", text: "Tim e Íris continuam a falar, mas com cautela. Aprendem devagar o que é confiar um no outro. A canção da cidade ainda espera por uma dança."},
  {min: -99, title: "Final: Silêncio", text: "Tim guarda a peça da armadura numa gaveta. Íris continua a existir, mas a distância entre os dois ficou. A pergunta fica no ar: o que torna uma relação verdadeira?"}
];
